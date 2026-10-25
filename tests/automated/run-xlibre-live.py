#!/usr/bin/env python3
"""Run the opt-in live XLibre output-property fixture."""
import argparse
import os
from pathlib import Path
import platform
import subprocess
import sys


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
               *xrandr_flags, str(source_root / "tests/automated/check-xlibre-live.c"),
               "-lXm", "-lXt", "-lXrandr", "-lX11", "-o", str(binary)]
    with (output / "compile.log").open("w") as stream:
        subprocess.run(command, env=env, check=True, stdout=stream,
                       stderr=subprocess.STDOUT)
    run = [str(binary)] if args.display else [
        "xvfb-run", "-a", "-s", "-screen 0 1024x768x24", str(binary)]
    result = subprocess.run(run, env=env, capture_output=True, text=True)
    (output / "run.log").write_text(result.stdout + result.stderr)
    if args.display:
        if result.returncode != 0 or "PASS XLibre live output-property transaction" not in result.stdout:
            print(result.stdout, end="", file=sys.stderr)
            print(result.stderr, end="", file=sys.stderr)
            return result.returncode or 1
    else:
        if result.returncode != 77 or "SKIP XLibre live observer" not in result.stdout:
            print(result.stdout, end="", file=sys.stderr)
            print(result.stderr, end="", file=sys.stderr)
            return result.returncode or 1
    print(result.stdout, end="")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
