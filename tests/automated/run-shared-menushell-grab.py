#!/usr/bin/env python3
"""Run shared XmMenuShell secondary-shell grab regression under Xvfb."""
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
    args = parser.parse_args()
    if platform.system() != "Linux":
        parser.error("run in an isolated Linux task workspace")
    prefix = args.prefix.resolve(strict=True)
    output = args.out.resolve()
    if not prefix.is_dir() or not output.is_absolute() or output.exists():
        parser.error("--out must be a fresh absolute directory")

    source_root = Path(__file__).resolve().parents[2]
    fixture = source_root / "tests/automated/check-shared-menushell-grab.c"
    output.mkdir(parents=True)

    env = dict(os.environ, LD_LIBRARY_PATH=str(prefix / "lib"), LC_ALL="C.UTF-8", TMPDIR=str(output))
    for name in ("DISPLAY", "LD_PRELOAD", "LD_AUDIT"):
        env.pop(name, None)

    binary = output / "check-shared-menushell-grab"
    cmd = [
        "cc", "-std=c99", "-Wall", "-Wextra", "-Werror", "-O2",
        "-I" + str(prefix / "include"),
        "-L" + str(prefix / "lib"), "-Wl,-rpath," + str(prefix / "lib"),
        str(fixture), "-lXm", "-lXt", "-lXtst", "-lX11", "-o", str(binary),
    ]
    compile_log = output / "compile.log"
    with compile_log.open("w") as stream:
        if subprocess.run(cmd, env=env, stdout=stream, stderr=subprocess.STDOUT).returncode:
            print(compile_log.read_text(errors="replace"), file=sys.stderr)
            return 1

    run_log = output / "run.log"
    with run_log.open("w") as stream:
        res = subprocess.run(
            ["xvfb-run", "-a", "-s", "-screen 0 1024x768x24", str(binary)],
            env=env, stdout=stream, stderr=subprocess.STDOUT,
        )
    text = run_log.read_text(errors="replace")
    if res.returncode or "PASS shared XmMenuShell press-drag-release" not in text:
        print(text, file=sys.stderr)
        return 1
    print(text, end="")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
