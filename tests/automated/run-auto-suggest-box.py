#!/usr/bin/env python3
"""Build and run the installed-prefix AutoSuggestBox contract probe on Xvfb."""
import os
from pathlib import Path
import subprocess
import sys


def main():
    prefix = Path(os.environ.get("MOTIF_TEST_PREFIX", ""))
    if not prefix.is_absolute():
        raise SystemExit("MOTIF_TEST_PREFIX must be an absolute installed prefix")
    root = Path(__file__).resolve().parents[2]
    temp = prefix / "tmp-auto-suggest-box"
    temp.mkdir(parents=True, exist_ok=True)
    binary = temp / "check-auto-suggest-box"
    env = dict(os.environ, LD_LIBRARY_PATH=str(prefix / "lib"),
               TMPDIR=str(temp), LC_ALL="C.UTF-8")
    flags = ["-std=c11", "-Wall", "-Wextra", "-Werror", "-g", "-O2",
             "-I" + str(prefix / "include"), "-L" + str(prefix / "lib"),
             "-Wl,-rpath," + str(prefix / "lib")]
    subprocess.run(["gcc", *flags,
                    str(root / "tests/automated/check-auto-suggest-box.c"),
                    "-lXm", "-lXt", "-lX11", "-o", str(binary)],
                   check=True, env=env)
    subprocess.run(["xvfb-run", "-a", str(binary)], check=True, env=env)


if __name__ == "__main__":
    sys.exit(main())
