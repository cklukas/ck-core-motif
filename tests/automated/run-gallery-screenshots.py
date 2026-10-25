#!/usr/bin/env python3
"""Capture manifest screenshot scenarios from a verified gallery window.

The runner is deliberately conservative: it never invents a screenshot when
the gallery binary, display, or xwd capture tool is unavailable.
"""
from __future__ import annotations

import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import time
import platform


WINDOW_NAME = "ckmotif-gallery"
WINDOW_RE = re.compile(
    r'^\s*(0x[0-9a-fA-F]+)\s+"' + re.escape(WINDOW_NAME) +
    r'":.*\s(\d+)x(\d+)[+-]\d+[+-]\d+\s'
)


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def process_failure(process: subprocess.Popen[bytes], entry_id: str) -> RuntimeError | None:
    """Return a diagnostic if the gallery exited before its shell appeared."""
    returncode = process.poll()
    if returncode is None:
        return None
    _, stderr = process.communicate()
    detail = stderr.decode(errors="replace").strip()
    suffix = f": {detail}" if detail else ""
    return RuntimeError(
        f"{entry_id}: gallery exited with status {returncode} before capture{suffix}"
    )


def stop_process(process: subprocess.Popen[bytes]) -> None:
    """Terminate a gallery child and always drain its stderr pipe."""
    if process.poll() is None:
        process.terminate()
        try:
            process.wait(timeout=3)
        except subprocess.TimeoutExpired:
            process.kill()
    process.communicate(timeout=3)


def find_window_id(xwininfo: str, env: dict[str, str], timeout: float = 10,
                   process: subprocess.Popen[bytes] | None = None,
                   entry_id: str = "gallery") -> str:
    deadline = time.monotonic() + timeout
    last_error = "window not found"
    while time.monotonic() < deadline:
        if process is not None:
            failure = process_failure(process, entry_id)
            if failure is not None:
                raise failure
        remaining = max(0.1, min(1.0, deadline - time.monotonic()))
        result = subprocess.run([xwininfo, "-root", "-tree"], env=env,
                                capture_output=True, text=True, timeout=remaining)
        if result.returncode:
            last_error = result.stderr.strip() or "xwininfo failed"
        else:
            candidates = []
            for line in result.stdout.splitlines():
                match = WINDOW_RE.match(line)
                if match:
                    width, height = (int(match.group(index)) for index in (2, 3))
                    if width > 1 and height > 1:
                        candidates.append((width * height, match.group(1)))
            if candidates:
                return max(candidates)[1]
            last_error = f"mapped {WINDOW_NAME} window not found"
        time.sleep(0.05)
    raise RuntimeError(last_error)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--binary", type=Path, required=True)
    parser.add_argument("--out", type=Path, required=True)
    parser.add_argument("--display", default=os.environ.get("DISPLAY"))
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    manifest_path = root / "docs/modern-gallery-manifest.json"
    manifest = json.loads(manifest_path.read_text())
    if not args.binary.is_absolute() or not args.out.is_absolute():
        parser.error("--binary and --out must be absolute")
    if (platform.system() == "Darwin" and
            not (str(args.out).startswith("/Volumes/S8T/tmp/") or
                 str(args.out).startswith("/Volumes/PRO-BLADE/tmp/"))):
        parser.error("Mac screenshot output must be under an approved temporary volume")
    if args.out.exists():
        parser.error(f"output already exists: {args.out}")
    args.out.mkdir(parents=True)
    xwd = shutil.which("xwd")
    xwininfo = shutil.which("xwininfo")
    xwdtopnm = shutil.which("xwdtopnm")
    pnmtopng = shutil.which("pnmtopng")
    env = dict(os.environ, DISPLAY=args.display, LC_ALL="C")
    receipt = {
        "schema": 1,
        "manifest": str(manifest_path),
        "manifest_sha256": sha256(manifest_path),
        "binary": str(args.binary),
        "binary_sha256": sha256(args.binary) if args.binary.is_file() else None,
        "display": args.display,
        "window_name": WINDOW_NAME,
        "output": str(args.out),
        "tools": {"xwd": xwd, "xwininfo": xwininfo,
                  "xwdtopnm": xwdtopnm, "pnmtopng": pnmtopng},
        "entries": [],
        "success": False,
        "result": "fail",
        "reason": None,
        "current_entry": None,
    }
    started = time.monotonic()

    def skip(reason: str) -> int:
        receipt["result"] = "skip"
        receipt["reason"] = reason
        print(f"skip: {reason}")
        return 77

    try:
        if not args.binary.is_file():
            return skip(f"gallery binary not found: {args.binary}")
        if not args.display:
            return skip("DISPLAY is not set")
        if not xwd or not xwininfo or not xwdtopnm or not pnmtopng:
            return skip("xwd, xwininfo, xwdtopnm and pnmtopng are required")
        for entry in manifest["entries"]:
            entry_started = time.monotonic()
            receipt["current_entry"] = entry["id"]
            process = subprocess.Popen(
                [str(args.binary), "--component", entry["id"], "--screenshot-mode"],
                env=env, cwd=root, stdout=subprocess.DEVNULL,
                stderr=subprocess.PIPE,
            )
            try:
                time.sleep(0.25)
                stem = entry["id"].replace(".", "-")
                xwd_output = args.out / (stem + ".xwd")
                png_output = args.out / (stem + ".png")
                window_id = find_window_id(xwininfo, env, process=process,
                                           entry_id=entry["id"])
                with xwd_output.open("wb") as stream:
                    capture = subprocess.run(
                        [xwd, "-id", window_id, "-silent"], env=env,
                        stdout=stream, stderr=subprocess.PIPE, timeout=10,
                    )
                if capture.returncode:
                    detail = capture.stderr.decode(errors="replace")
                    raise RuntimeError(f"{entry['id']}: xwd failed: {detail}")
                pnm = subprocess.Popen([xwdtopnm, str(xwd_output)],
                                       stdout=subprocess.PIPE,
                                       stderr=subprocess.PIPE)
                with png_output.open("wb") as stream:
                    convert = subprocess.run([pnmtopng], stdin=pnm.stdout,
                                             stdout=stream, stderr=subprocess.PIPE,
                                             timeout=10)
                if pnm.stdout is not None:
                    pnm.stdout.close()
                pnm_stderr = pnm.stderr.read().decode(errors="replace")
                pnm_returncode = pnm.wait(timeout=10)
                if convert.returncode or pnm_returncode:
                    raise RuntimeError(
                        f"{entry['id']}: PNG conversion failed: "
                        f"{pnm_stderr}{convert.stderr.decode(errors='replace')}"
                    )
                receipt["entries"].append({
                    "id": entry["id"],
                    "xwd": str(xwd_output),
                    "png": str(png_output),
                    "xwd_sha256": sha256(xwd_output),
                    "png_sha256": sha256(png_output),
                    "seconds": round(time.monotonic() - entry_started, 3),
                })
            finally:
                stop_process(process)
            receipt["current_entry"] = None
        receipt["success"] = True
        receipt["result"] = "pass"
        print(f"gallery screenshots: {len(manifest['entries'])} scenarios captured")
        return 0
    except (OSError, RuntimeError, subprocess.TimeoutExpired) as error:
        receipt["reason"] = str(error)
        print(f"gallery screenshots: {error}", file=sys.stderr)
        return 1
    finally:
        receipt["ended_utc"] = datetime.datetime.now(datetime.timezone.utc).isoformat()
        receipt["seconds"] = round(time.monotonic() - started, 3)
        (args.out / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n")


if __name__ == "__main__":
    raise SystemExit(main())
