#!/usr/bin/env python3
"""Run the optional installed XLibre RandR DPI contract fixture."""
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


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--prefix", required=True, type=Path)
    parser.add_argument("--out", required=True, type=Path)
    parser.add_argument("--display", help="use an existing XLibre display")
    parser.add_argument("--xrandr-root", type=Path,
                        help="staged Xrandr development prefix, if needed")
    args = parser.parse_args()
    if platform.system() != "Linux":
        parser.error("run in an isolated Linux task workspace")
    prefix = args.prefix.resolve(strict=True)
    output = args.out.resolve()
    if not prefix.is_absolute() or not output.is_absolute() or output.exists():
        parser.error("--out must be a fresh absolute directory")
    source_root = Path(__file__).resolve().parents[2]
    fixture = source_root / "tests/automated/check-xlibre-dpi-contract.c"
    header_root = source_root / "lib/Xm"
    output.mkdir(parents=True)
    source_files = [fixture, selection_fixture, observer_fixture,
                    source_root / "tests/automated/check-xlibre-lifetime.py"]
    receipt = {
        "source_root": str(source_root),
        "prefix": str(prefix),
        "output": str(output),
        "display": args.display,
        "source_sha256": {
            str(path.relative_to(source_root)): hashlib.sha256(
                path.read_bytes()).hexdigest() for path in source_files
        },
        "steps": [],
        "success": False,
    }
    started = time.monotonic()
    env = dict(os.environ, LD_LIBRARY_PATH=str(prefix / "lib"),
               LC_ALL="C.UTF-8", TMPDIR=str(output))
    for name in ("DISPLAY", "LD_PRELOAD", "LD_AUDIT"):
        env.pop(name, None)
    if args.display:
        env["DISPLAY"] = args.display
    binary = output / "check-xlibre-dpi-contract"
    xrandr_flags = []
    if args.xrandr_root:
        xrandr_root = args.xrandr_root.resolve(strict=True)
        xrandr_lib_dirs = [xrandr_root / "usr/lib"]
        xrandr_lib_dirs.extend(path for path in (xrandr_root / "usr/lib").glob("*")
                               if path.is_dir())
        xrandr_flags = [
            "-I" + str(xrandr_root / "usr/include"),
            *["-L" + str(path) for path in xrandr_lib_dirs],
        ]
    command = [
        "cc", "-std=c99", "-Wall", "-Wextra", "-Werror", "-O2",
        "-I" + str(prefix / "include"), "-I" + str(header_root),
        "-L" + str(prefix / "lib"), "-Wl,-rpath," + str(prefix / "lib"),
        *xrandr_flags, str(fixture), "-lXm", "-lXt", "-lXrandr", "-lX11",
        "-o", str(binary),
    ]
    receipt["steps"].append({"name": "compile-contract", "command": command})
    compile_log = output / "compile.log"
    with compile_log.open("w") as stream:
        if subprocess.run(command, env=env, stdout=stream,
                          stderr=subprocess.STDOUT).returncode:
            print(compile_log.read_text(errors="replace"), file=sys.stderr)
            return 1
    selection_command = [
        "cc", "-std=c99", "-Wall", "-Wextra", "-Werror", "-O2",
        "-I" + str(prefix / "include"), "-I" + str(header_root),
        "-L" + str(prefix / "lib"), "-Wl,-rpath," + str(prefix / "lib"),
        *xrandr_flags, str(selection_fixture), "-lXm", "-lXt", "-lXrandr",
        "-lX11", "-o", str(selection_binary),
    ]
    receipt["steps"].append({"name": "compile-selection",
                              "command": selection_command})
    with compile_log.open("a") as stream:
        if subprocess.run(selection_command, env=env, stdout=stream,
                          stderr=subprocess.STDOUT).returncode:
            print(compile_log.read_text(errors="replace"), file=sys.stderr)
            return 1
    observer_command = [
        "cc", "-std=c99", "-Wall", "-Wextra", "-Werror", "-O2",
        "-I" + str(prefix / "include"), "-I" + str(header_root),
        "-L" + str(prefix / "lib"), "-Wl,-rpath," + str(prefix / "lib"),
        *xrandr_flags, str(observer_fixture), "-lXm", "-lXt", "-lXrandr",
        "-lX11", "-o", str(observer_binary),
    ]
    receipt["steps"].append({"name": "compile-observer",
                              "command": observer_command})
    with compile_log.open("a") as stream:
        if subprocess.run(observer_command, env=env, stdout=stream,
                          stderr=subprocess.STDOUT).returncode:
            print(compile_log.read_text(errors="replace"), file=sys.stderr)
            return 1
>>>>>>> theirs
    run = [str(binary)] if args.display else [
        "xvfb-run", "-a", "-s", "-screen 0 1024x768x24", str(binary)
    ]
    receipt["steps"].append({"name": "run-contract", "command": run,
                              "log": str(output / "run.log")})
    run_log = output / "run.log"
    with run_log.open("w") as stream:
        if subprocess.run(run, env=env, stdout=stream,
                          stderr=subprocess.STDOUT).returncode:
            print(run_log.read_text(errors="replace"), file=sys.stderr)
            return 1
    selection_log = output / "selection.log"
    with selection_log.open("w") as stream:
        if subprocess.run([str(selection_binary)], env=env, stdout=stream,
                          stderr=subprocess.STDOUT).returncode:
            print(selection_log.read_text(errors="replace"), file=sys.stderr)
            return 1
    receipt["steps"].append({"name": "run-selection",
                              "command": [str(selection_binary)],
                              "log": str(selection_log)})
    observer_log = output / "observer.log"
    observer_run = [str(observer_binary)] if args.display else [
        "xvfb-run", "-a", "-s", "-screen 0 1024x768x24", str(observer_binary)
    ]
    receipt["steps"].append({"name": "run-observer", "command": observer_run,
                              "log": str(observer_log)})
    with observer_log.open("w") as stream:
        if subprocess.run(observer_run, env=env, stdout=stream,
                          stderr=subprocess.STDOUT).returncode:
            print(observer_log.read_text(errors="replace"), file=sys.stderr)
            return 1
    result = run_log.read_text(errors="replace")
    if "PASS XLibre RandR DPI contract and non-XLibre fallback" not in result:
        print(result, file=sys.stderr)
        return 1
    library = prefix / "lib/libXm.so.8.0.0"
    if library.exists():
        receipt["libXm_sha256"] = hashlib.sha256(library.read_bytes()).hexdigest()
    receipt["success"] = True
    receipt["elapsed_seconds"] = round(time.monotonic() - started, 3)
    receipt["ended_utc"] = datetime.datetime.now(datetime.timezone.utc).isoformat()
    (output / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n")
    print(result, end="")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
