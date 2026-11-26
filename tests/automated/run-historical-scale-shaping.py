#!/usr/bin/env python3
"""Run historical-widget DPI reflow and sub-paragraph incremental shaping suite."""
import argparse
import os
from pathlib import Path
import subprocess
import sys


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--prefix", required=True, type=Path)
    parser.add_argument("--out", required=True, type=Path)
    args = parser.parse_args()

    prefix = args.prefix.resolve(strict=True)
    out = args.out.resolve()
    if out.exists():
        parser.error("--out must be a fresh directory")
    out.mkdir(parents=True)

    src_root = Path(__file__).resolve().parents[2]
    fixture = src_root / "tests/automated/check-historical-scale-shaping.c"
    binary = out / "check-historical-scale-shaping"

    pkg_cflags = subprocess.check_output(
        ["pkg-config", "--cflags", "xft", "fontconfig", "freetype2"], text=True
    ).split()
    pkg_libs = subprocess.check_output(
        ["pkg-config", "--libs", "xft", "fontconfig"], text=True
    ).split()

    cmd = [
        "cc", "-std=c11", "-Wall", "-Wextra", "-Werror", "-O2", "-g",
        "-DUSE_XFT=1",
        f"-I{prefix / 'include'}",
        f"-I{src_root / 'lib/Xm'}",
        *pkg_cflags,
        str(fixture),
        f"-L{prefix / 'lib'}",
        f"-Wl,-rpath,{prefix / 'lib'}",
        "-lXm", "-lXt", "-lX11", *pkg_libs,
        "-lm", "-o", str(binary),
    ]
    subprocess.run(cmd, check=True)

    env = dict(
        os.environ,
        LC_ALL="C.UTF-8",
        TMPDIR=str(out),
        LD_LIBRARY_PATH=str(prefix / "lib"),
    )
    env.pop("DISPLAY", None)
    res = subprocess.run(
        ["xvfb-run", "-a", "-s", "-screen 0 1280x1024x24", str(binary)],
        env=env,
        capture_output=True,
        text=True,
    )
    (out / "run.log").write_text(res.stdout + res.stderr)
    if res.returncode != 0:
        print(res.stdout + res.stderr, file=sys.stderr)
        return res.returncode
    print(res.stdout, end="")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
