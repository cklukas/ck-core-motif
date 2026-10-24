#!/usr/bin/env python3
"""Run the installed-library WP-01 appearance-profile contract test."""
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
    args = parser.parse_args()
    if platform.system() != "Linux":
        parser.error("WP-01 currently requires Linux/Xvfb")
    prefix = args.prefix.resolve(strict=True)
    output = args.out.resolve()
    if not prefix.is_absolute() or not output.is_absolute() or output.exists():
        parser.error("--out must be a fresh absolute directory")
    source_root = Path(__file__).resolve().parents[2]
    fixture = source_root / "tests/automated/check-appearance-profile.c"
    output.mkdir(parents=True)
    env = dict(os.environ, LD_LIBRARY_PATH=str(prefix / "lib"),
               LC_ALL="C.UTF-8", TMPDIR=str(output))
    for name in ("DISPLAY", "LD_PRELOAD", "LD_AUDIT"):
        env.pop(name, None)
    binary = output / "check-appearance-profile"
    compile_command = [
        "cc", "-std=c99", "-Wall", "-Wextra", "-Werror", "-O2",
        "-I" + str(prefix / "include"), "-L" + str(prefix / "lib"),
        "-Wl,-rpath," + str(prefix / "lib"), str(fixture), "-lXm", "-lXt",
        "-lX11", "-o", str(binary),
    ]
    compile_log = output / "compile.log"
    with compile_log.open("w") as stream:
        if subprocess.run(compile_command, env=env, stdout=stream,
                          stderr=subprocess.STDOUT).returncode:
            print(compile_log.read_text(errors="replace"), file=sys.stderr)
            return 1
    run_log = output / "run.log"
    run_command = ["xvfb-run", "-a", "-s", "-screen 0 1024x768x24",
                   str(binary), "-xrm", "AppearanceProfileTest.width: 641"]
    with run_log.open("w") as stream:
        if subprocess.run(run_command, env=env, stdout=stream,
                          stderr=subprocess.STDOUT).returncode:
            print(run_log.read_text(errors="replace"), file=sys.stderr)
            return 1
    result = run_log.read_text(errors="replace")
    if "PASS WP-01 classic appearance profile" not in result:
        print(result, file=sys.stderr)
        return 1
    print(result, end="")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
