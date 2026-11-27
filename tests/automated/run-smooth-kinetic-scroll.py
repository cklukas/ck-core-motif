#!/usr/bin/env python3
"""Compile and run XInput2 smooth-scroll & sub-line pixel scrolling qualification."""
from __future__ import annotations

import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import platform
import subprocess
import time


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--prefix", required=True, type=Path)
    parser.add_argument("--out", required=True, type=Path)
    args = parser.parse_args()

    if platform.system() != "Linux":
        parser.error("XInput2 smooth-scroll qualification requires Linux/Xvfb")
    prefix = args.prefix.resolve(strict=True)
    output = args.out.resolve()
    if output.exists() or output.is_relative_to(prefix) or prefix.is_relative_to(output):
        parser.error("--out must be fresh and separate from --prefix")

    output.mkdir(parents=True)
    (output / "tmp").mkdir()
    source = Path(__file__).with_name("check-smooth-kinetic-scroll.c").resolve()
    binary = output / "check-smooth-kinetic-scroll"
    library = (prefix / "lib/libXm.so").resolve(strict=True)

    env = dict(
        os.environ,
        TMPDIR=str(output / "tmp"),
        LC_ALL="C.UTF-8",
        LD_LIBRARY_PATH=str(prefix / "lib"),
        PYTHONDONTWRITEBYTECODE="1",
    )
    env.pop("DISPLAY", None)
    env.pop("WAYLAND_DISPLAY", None)

    started = time.monotonic()
    receipt: dict = {
        "library": str(library),
        "library_sha256": sha256(library),
        "source": str(source),
        "source_sha256": sha256(source),
        "steps": [],
        "success": False,
    }
    try:
        cres = subprocess.run(
            [
                "cc", "-std=c11", "-Wall", "-Wextra", "-Werror", "-O2", "-g",
                "-rdynamic",
                "-I" + str(prefix / "include"),
                "-L" + str(prefix / "lib"),
                "-Wl,-rpath," + str(prefix / "lib"),
                str(source), "-lXm", "-lXt", "-lX11", "-lXtst", "-lm",
                "-o", str(binary),
            ],
            env=env,
            capture_output=True,
            text=True,
            timeout=60,
        )
        (output / "compile.log").write_text(cres.stdout + cres.stderr)
        receipt["steps"].append({"name": "compile", "exit": cres.returncode})
        if cres.returncode != 0:
            print(cres.stdout + cres.stderr)
            return 1

        rres = subprocess.run(
            ["xvfb-run", "-a", "-s", "-screen 0 1280x960x24", str(binary)],
            env=env,
            capture_output=True,
            text=True,
            timeout=60,
        )
        (output / "run.log").write_text(rres.stdout + rres.stderr)
        receipt["steps"].append({"name": "run", "exit": rres.returncode})
        if rres.returncode != 0 or "PASS smooth/kinetic XInput2" not in rres.stdout:
            print(rres.stdout + rres.stderr)
            return 1
        receipt["success"] = True
    finally:
        receipt["ended_utc"] = datetime.datetime.now(datetime.timezone.utc).isoformat()
        receipt["seconds"] = round(time.monotonic() - started, 3)
        (output / "receipt.json").write_text(json.dumps(receipt, indent=2) + "\n")

    print(rres.stdout.strip())
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
