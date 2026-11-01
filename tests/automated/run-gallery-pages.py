#!/usr/bin/env python3
"""Verify welcome/icon page reflow against an explicit native gallery build."""
import argparse
import datetime
import hashlib
import os
from pathlib import Path
import subprocess
import signal
import time
import json

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--binary", required=True, type=Path)
parser.add_argument("--prefix", required=True, type=Path)
parser.add_argument("--out", required=True, type=Path)
parser.add_argument("--expect-no-svg", action="store_true")
args = parser.parse_args()
binary = args.binary.resolve(strict=True)
prefix = args.prefix.resolve(strict=True)
output = args.out.resolve()
output.mkdir(parents=True, exist_ok=False)
(output / "tmp").mkdir()
sha = lambda path: hashlib.sha256(path.read_bytes()).hexdigest()
library = prefix / "lib/libXm.so.9.0.0"
env = dict(
    os.environ,
    TMPDIR=str(output / "tmp"),
    LD_LIBRARY_PATH=str(prefix / "lib"),
    XFILESEARCHPATH=str(output / "%N"),
    XUSERFILESEARCHPATH=str(output / "%N"),
)
for name in (
    "LD_PRELOAD",
    "LD_AUDIT",
    "XENVIRONMENT",
    "XAPPLRESDIR",
    "XM_EXPECT_SVG_UNAVAILABLE",
):
    env.pop(name, None)
if args.expect_no_svg:
    env["XM_EXPECT_SVG_UNAVAILABLE"] = "1"
source = (
    Path(__file__).resolve().parents[2] / "demos/programs/ckmotif-gallery/gallery.c"
)
record = {
    "started_utc": datetime.datetime.now(datetime.timezone.utc).isoformat(),
    "binary": str(binary),
    "binary_sha256": sha(binary),
    "library": str(library),
    "library_sha256": sha(library),
    "source_sha256": sha(source),
    "expected_svg": not args.expect_no_svg,
    "success": False,
    "steps": [],
}
for process_path in Path("/proc").iterdir():
    if not process_path.name.isdigit():
        continue
    try:
        name = (process_path / "comm").read_text().strip()
    except OSError:
        continue
    if name in ("ckmotif-gallery", "periodic"):
        raise RuntimeError("Existing project app: " + process_path.name)
started = time.monotonic()
try:
    linked = subprocess.run(
        ["ldd", str(binary)], env=env, check=True, capture_output=True, text=True
    ).stdout
    (output / "link.log").write_text(linked)
    if str(prefix / "lib/libXm.so") not in linked:
        raise RuntimeError("gallery did not resolve the selected library")
    for profile in ("classic", "modern-light", "modern-dark"):
        for scale in ("1", "1.5", "2"):
            command = [
                "xvfb-run",
                "-a",
                "-s",
                "-screen 0 2400x1700x24",
                str(binary),
                "--integration-pages",
                "--appearance",
                profile,
                "--scale",
                scale,
            ]
            began = time.monotonic()
            process = subprocess.Popen(
                command,
                env=env,
                cwd=source.parents[3],
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True,
                start_new_session=True,
            )
            try:
                stdout, stderr = process.communicate(timeout=90)
            except BaseException:
                try:
                    os.killpg(process.pid, signal.SIGTERM)
                except ProcessLookupError:
                    pass
                try:
                    process.communicate(timeout=5)
                except subprocess.TimeoutExpired:
                    try:
                        os.killpg(process.pid, signal.SIGKILL)
                    except ProcessLookupError:
                        pass
                    process.communicate()
                raise
            result = subprocess.CompletedProcess(
                command, process.returncode, stdout, stderr
            )
            log = result.stdout + result.stderr
            (output / (profile + "-" + scale + ".log")).write_text(log)
            record["steps"].append(
                {
                    "command": command,
                    "exit": result.returncode,
                    "seconds": time.monotonic() - began,
                }
            )
            if (result.returncode or "PASS gallery flow pages: 10" not in log
                    or "Abandoned edge synchronization" in log):
                raise RuntimeError(log)
            print("PASS", profile, scale, flush=True)
    if (
        sha(binary) != record["binary_sha256"]
        or sha(library) != record["library_sha256"]
    ):
        raise RuntimeError("build changed during qualification")
    record["success"] = True
finally:
    record["ended_utc"] = datetime.datetime.now(datetime.timezone.utc).isoformat()
    record["seconds"] = time.monotonic() - started
    (output / "receipt.json").write_text(json.dumps(record, indent=2))
print("PASS gallery flow pages: 90 native resize/content/icon scenarios")
