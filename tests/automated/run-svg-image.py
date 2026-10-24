#!/usr/bin/env python3
"""Run the optional installed SVG image-cache regression."""
import argparse
import os
from pathlib import Path
import platform
import subprocess
import sys


VALID_SVG = """<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"16\" height=\"12\" viewBox=\"0 0 16 12\"><rect width=\"16\" height=\"12\" fill=\"#336699\"/><circle cx=\"8\" cy=\"6\" r=\"4\" fill=\"#ffffff\"/></svg>\n"""
INVALID_SVG = "<svg xmlns=\"http://www.w3.org/2000/svg\"><path>\n"


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--prefix", required=True, type=Path)
    parser.add_argument("--out", required=True, type=Path)
    parser.add_argument("--expect-unavailable", action="store_true",
                        help="verify the build rejects SVG input")
    args = parser.parse_args()
    if platform.system() != "Linux":
        parser.error("run in an isolated Linux task workspace")
    prefix = args.prefix.resolve(strict=True)
    output = args.out.resolve()
    if not prefix.is_dir() or not output.is_absolute() or output.exists():
        parser.error("--out must be a fresh absolute directory")
    source_root = Path(__file__).resolve().parents[2]
    fixture = source_root / "tests/automated/check-svg-image.c"
    output.mkdir(parents=True)
    valid = output / "valid.svg"
    invalid = output / "invalid.svg"
    valid.write_text(VALID_SVG, encoding="utf-8")
    invalid.write_text(INVALID_SVG, encoding="utf-8")
    env = dict(os.environ, LD_LIBRARY_PATH=str(prefix / "lib"),
               LC_ALL="C.UTF-8", TMPDIR=str(output))
    if args.expect_unavailable:
        env["XM_EXPECT_SVG_UNAVAILABLE"] = "1"
    for name in ("DISPLAY", "LD_PRELOAD", "LD_AUDIT"):
        env.pop(name, None)
    binary = output / "check-svg-image"
    command = [
        "cc", "-std=c99", "-Wall", "-Wextra", "-Werror", "-O2",
        "-I" + str(prefix / "include"), "-I" + str(source_root / "lib/Xm"),
        "-L" + str(prefix / "lib"), "-Wl,-rpath," + str(prefix / "lib"),
        str(fixture), "-lXm", "-lXt", "-lX11", "-o", str(binary),
    ]
    compile_log = output / "compile.log"
    with compile_log.open("w") as stream:
        if subprocess.run(command, env=env, stdout=stream,
                          stderr=subprocess.STDOUT).returncode:
            print(compile_log.read_text(errors="replace"), file=sys.stderr)
            return 1
    run_log = output / "run.log"
    with run_log.open("w") as stream:
        result = subprocess.run(
            ["xvfb-run", "-a", "-s", "-screen 0 1024x768x24", str(binary),
             str(valid), str(invalid)],
            env=env, stdout=stream, stderr=subprocess.STDOUT)
    text = run_log.read_text(errors="replace")
    marker = ("PASS SVG unavailable" if args.expect_unavailable
              else "PASS SVG image loading")
    if result.returncode or marker not in text:
        print(text, file=sys.stderr)
        return 1
    print(text, end="")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
