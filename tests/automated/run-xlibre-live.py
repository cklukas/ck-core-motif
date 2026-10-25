#!/usr/bin/env python3
"""Run the opt-in live XLibre output-property fixture."""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import platform
import subprocess
import sys
import time


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--prefix", required=True, type=Path)
    parser.add_argument("--out", required=True, type=Path)
    parser.add_argument("--display", help="real XLibre display; omit for fallback check")
    parser.add_argument("--xrandr-root", type=Path)
    args = parser.parse_args()
    if platform.system() != "Linux":
        parser.error("run in an isolated Linux task workspace")
    prefix = args.prefix.resolve(strict=True)
    output = args.out.resolve()
    if not prefix.is_absolute() or not output.is_absolute() or output.exists():
        parser.error("--out must be a fresh absolute directory")
    source_root = Path(__file__).resolve().parents[2]
    output.mkdir(parents=True)
    fixture = source_root / "tests/automated/check-xlibre-live.c"
    header = source_root / "lib/Xm/XlibreDpiI.h"
    installed_lib = prefix / "lib/libXm.so.8.0.0"
    if not installed_lib.is_file():
        parser.error(f"installed libXm not found: {installed_lib}")
    started = time.monotonic()
    receipt = {
        "schema": 1,
        "source_hashes": {
            str(fixture): sha256(fixture),
            str(header): sha256(header),
            str(Path(__file__).resolve()): sha256(Path(__file__).resolve()),
        },
        "prefix": str(prefix),
        "installed_lib": str(installed_lib),
        "installed_lib_sha256": sha256(installed_lib),
        "output": str(output),
        "display": args.display,
        "xrandr_root": str(args.xrandr_root.resolve(strict=True))
        if args.xrandr_root else None,
        "mode": "real-display" if args.display else "ordinary-xvfb-fallback",
        "result": "pending",
        "steps": [],
        "success": False,
    }
    env = dict(os.environ, LC_ALL="C.UTF-8", TMPDIR=str(output),
               LD_LIBRARY_PATH=str(prefix / "lib"))
    for name in ("DISPLAY", "LD_PRELOAD", "LD_AUDIT"):
        env.pop(name, None)
    if args.display:
        env["DISPLAY"] = args.display
    xrandr_flags = []
    if args.xrandr_root:
        root = args.xrandr_root.resolve(strict=True)
        lib_dirs = [root / "usr/lib"]
        lib_dirs.extend(path for path in (root / "usr/lib").glob("*")
                        if path.is_dir())
        xrandr_flags = ["-I" + str(root / "usr/include"),
                        *["-L" + str(path) for path in lib_dirs]]
    binary = output / "check-xlibre-live"
    command = ["cc", "-std=c11", "-Wall", "-Wextra", "-Werror", "-O2", "-g",
               "-I" + str(prefix / "include"), "-I" + str(source_root / "lib/Xm"),
               "-L" + str(prefix / "lib"), "-Wl,-rpath," + str(prefix / "lib"),
               *xrandr_flags, str(fixture),
               "-lXm", "-lXt", "-lXrandr", "-lX11", "-o", str(binary)]
    try:
        compile_started = time.monotonic()
        with (output / "compile.log").open("w") as stream:
            compile_result = subprocess.run(command, env=env,
                                             stdout=stream,
                                             stderr=subprocess.STDOUT)
        receipt["steps"].append({
            "name": "compile",
            "command": command,
            "exit": compile_result.returncode,
            "seconds": round(time.monotonic() - compile_started, 3),
            "log": str(output / "compile.log"),
        })
        if compile_result.returncode:
            raise RuntimeError(f"compile failed with exit {compile_result.returncode}")
        run = [str(binary)] if args.display else [
            "xvfb-run", "-a", "-s", "-screen 0 1024x768x24", str(binary)]
        run_started = time.monotonic()
        result = subprocess.run(run, env=env, capture_output=True, text=True)
        (output / "run.log").write_text(result.stdout + result.stderr)
        receipt["steps"].append({
            "name": "run",
            "command": run,
            "exit": result.returncode,
            "seconds": round(time.monotonic() - run_started, 3),
            "log": str(output / "run.log"),
        })
        if args.display:
            expected = (result.returncode == 0 and
                        "PASS XLibre live output-property transaction" in
                        result.stdout)
        else:
            expected = (result.returncode == 77 and
                        "SKIP XLibre live observer" in result.stdout)
        if not expected:
            receipt["result"] = "fail"
            print(result.stdout, end="", file=sys.stderr)
            print(result.stderr, end="", file=sys.stderr)
            return result.returncode or 1
        receipt["success"] = True
        receipt["result"] = "pass" if args.display else "skip"
        print(result.stdout, end="")
        return 0
    except (OSError, RuntimeError) as error:
        receipt["result"] = "fail"
        print(f"XLibre live fixture: {error}", file=sys.stderr)
        return 1
    finally:
        receipt["ended_utc"] = datetime.datetime.now(datetime.timezone.utc).isoformat()
        receipt["seconds"] = round(time.monotonic() - started, 3)
        (output / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n")


if __name__ == "__main__":
    raise SystemExit(main())
