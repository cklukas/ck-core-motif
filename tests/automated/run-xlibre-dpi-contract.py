#!/usr/bin/env python3
"""Run the optional installed XLibre RandR DPI contract fixture."""
import argparse
import os
from pathlib import Path
import platform
import subprocess
import sys


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
    selection_fixture = source_root / "tests/automated/check-xlibre-output-selection.c"
    header_root = source_root / "lib/Xm"
    output.mkdir(parents=True)
    env = dict(os.environ, LD_LIBRARY_PATH=str(prefix / "lib"),
               LC_ALL="C.UTF-8", TMPDIR=str(output))
    for name in ("DISPLAY", "LD_PRELOAD", "LD_AUDIT"):
        env.pop(name, None)
    if args.display:
        env["DISPLAY"] = args.display
    binary = output / "check-xlibre-dpi-contract"
    selection_binary = output / "check-xlibre-output-selection"
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
    with compile_log.open("a") as stream:
        if subprocess.run(selection_command, env=env, stdout=stream,
                          stderr=subprocess.STDOUT).returncode:
            print(compile_log.read_text(errors="replace"), file=sys.stderr)
            return 1
    run = [str(binary)] if args.display else [
        "xvfb-run", "-a", "-s", "-screen 0 1024x768x24", str(binary)
    ]
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
    result = run_log.read_text(errors="replace")
    selection_result = selection_log.read_text(errors="replace")
    if ("PASS XLibre RandR DPI contract and non-XLibre fallback" not in result or
            "PASS XLibre shell output selection policy" not in selection_result):
        print(result, file=sys.stderr)
        print(selection_result, file=sys.stderr)
        return 1
    print(result, end="")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
