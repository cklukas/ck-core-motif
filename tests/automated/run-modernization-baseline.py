#!/usr/bin/env python3
"""Run the installed-library WP-00 modernization baseline on Xvfb."""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import platform
import signal
import subprocess
import sys
import time


def sha256(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--prefix", required=True, type=Path,
                        help="absolute installed toolkit prefix")
    parser.add_argument("--out", required=True, type=Path,
                        help="fresh absolute results/scratch directory")
    parser.add_argument("--cc", default="cc", help="native C compiler")
    parser.add_argument("--dpi", default="96", help="Xft.dpi baseline resource")
    args = parser.parse_args()
    if platform.system() != "Linux":
        parser.error("WP-00 currently requires Linux/Xvfb")
    if not args.prefix.is_absolute() or not args.out.is_absolute():
        parser.error("--prefix and --out must be absolute")
    prefix = args.prefix.resolve(strict=True)
    output = args.out.resolve()
    if output.exists() or output.is_relative_to(prefix) or prefix.is_relative_to(output):
        parser.error("--out must be fresh and separate from --prefix")
    if not args.dpi.isdigit() or not (1 <= int(args.dpi) <= 960):
        parser.error("--dpi must be an integer from 1 through 960")
    source_root = Path(__file__).resolve().parents[2]
    fixture = source_root / "tests/modernization/baseline.c"
    output.mkdir(parents=True)
    for name in ("tmp", "cache", "config"):
        (output / name).mkdir()
    resource_file = output / "resources"
    resource_file.write_text(f"Xft.dpi: {args.dpi}\n")
    env = dict(os.environ, TMPDIR=str(output / "tmp"),
               XDG_CACHE_HOME=str(output / "cache"),
               XDG_CONFIG_HOME=str(output / "config"),
               LC_ALL="C.UTF-8", LD_LIBRARY_PATH=str(prefix / "lib"),
               XENVIRONMENT=str(resource_file),
               PYTHONDONTWRITEBYTECODE="1")
    for name in ("DISPLAY", "LD_PRELOAD", "LD_AUDIT"):
        env.pop(name, None)
    receipt = {
        "started_utc": datetime.datetime.now(datetime.timezone.utc).isoformat(),
        "prefix": str(prefix), "output": str(output), "dpi": int(args.dpi),
        "architecture": platform.machine(), "source": str(fixture),
        "source_sha256": sha256(fixture), "steps": [], "success": False,
        "scope": "Installed-library native consumer under Xvfb; actual-window PPM capture",
    }
    began = time.monotonic()

    def run(name, command, timeout=120):
        command = [str(value) for value in command]
        started = time.monotonic()
        log = output / (name + ".log")
        with log.open("w") as stream:
            process = subprocess.Popen(command, cwd=output, env=env,
                                       stdout=stream, stderr=subprocess.STDOUT,
                                       start_new_session=True)
            try:
                code = process.wait(timeout=timeout)
            except subprocess.TimeoutExpired:
                os.killpg(process.pid, signal.SIGTERM)
                try:
                    process.wait(timeout=5)
                except subprocess.TimeoutExpired:
                    os.killpg(process.pid, signal.SIGKILL)
                    process.wait()
                code = 124
        receipt["steps"].append({"name": name, "command": command,
                                 "exit": code,
                                 "seconds": round(time.monotonic() - started, 3),
                                 "log": str(log)})
        if code:
            print(log.read_text(errors="replace"), file=sys.stderr)
            raise RuntimeError(f"{name} failed with exit {code}")
        return log.read_text(errors="replace")

    try:
        library = (prefix / "lib/libXm.so.8").resolve(strict=True)
        if not library.is_relative_to(prefix):
            raise RuntimeError("libXm.so.8 escapes selected prefix")
        receipt["library"] = {"path": str(library), "sha256": sha256(library)}
        binary = output / "modernization-baseline"
        dependency = output / "modernization-baseline.d"
        flags = ["-std=c99", "-Wall", "-Wextra", "-Werror", "-O2", "-g",
                 "-MD", "-MF", str(dependency), "-I" + str(prefix / "include"),
                 "-L" + str(prefix / "lib"), "-Wl,-rpath," + str(prefix / "lib")]
        run("compile", [args.cc, *flags, fixture, "-lXm", "-lXt", "-lX11",
                         "-o", binary])
        linked = run("link", ["ldd", binary])
        if str(prefix / "lib/libXm.so.8") not in linked:
            raise RuntimeError("baseline did not resolve libXm from selected prefix")
        result = output / "baseline.txt"
        image = output / "baseline.ppm"
        command = ["xvfb-run", "-a", "-s", "-screen 0 1024x768x24 -dpi " + args.dpi,
                   "sh", "-c", "exec \"$2\" \"$3\" \"$4\"",
                   "baseline", args.dpi, binary, result, image]
        log = run("run", command)
        if "PASS WP-00 modernization baseline" not in log:
            raise RuntimeError("baseline did not report PASS")
        lines = result.read_text().splitlines()
        if not lines or lines[0] != "SCHEMA 1":
            raise RuntimeError("baseline output has no recognized schema")
        if not image.read_bytes().startswith(b"P6\n"):
            raise RuntimeError("baseline image is not a PPM capture")
        receipt["result_sha256"] = sha256(result)
        receipt["image_sha256"] = sha256(image)
        receipt["success"] = True
    finally:
        receipt["ended_utc"] = datetime.datetime.now(datetime.timezone.utc).isoformat()
        receipt["seconds"] = round(time.monotonic() - began, 3)
        (output / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, RuntimeError) as error:
        print(f"ERROR: {error}", file=sys.stderr)
        raise SystemExit(1)
