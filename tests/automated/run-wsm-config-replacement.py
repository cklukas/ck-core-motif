#!/usr/bin/env python3
"""Build and run the WSM configuration-replacement fixture under Xvfb."""

import argparse
import os
from pathlib import Path
import subprocess


parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--build-root", required=True, type=Path)
parser.add_argument("--out", required=True, type=Path)
args = parser.parse_args()

source = Path(__file__).resolve().parents[2]
build = args.build_root.resolve()
out = args.out.resolve()
if out.exists():
    parser.error("--out must be a fresh directory")
out.mkdir(parents=True)
tmp = out / "tmp"
tmp.mkdir()
environment = dict(os.environ, TMPDIR=str(tmp), PYTHONDONTWRITEBYTECODE="1")
fixture = out / "wsm-config-replacement"

compile_command = [
    os.environ.get("CC", "cc"), "-std=c99", "-Wall", "-Wextra", "-Werror",
    "-I" + str(build / "lib"), "-I" + str(source / "lib"),
    "-I" + str(source / "clients/mwm/WmWsmLib"),
    str(source / "tests/automated/wsm-config-replacement.c"),
    str(build / "clients/mwm/WmWsmLib/libWsm.a"),
    "-lXt", "-lX11", "-o", str(fixture),
]
subprocess.run(compile_command, check=True, env=environment)
result = subprocess.run(["xvfb-run", "-a", str(fixture)], check=True,
                        env=environment, text=True, capture_output=True)
(out / "result.log").write_text(result.stdout + result.stderr, encoding="utf-8")
print(result.stdout, end="")
