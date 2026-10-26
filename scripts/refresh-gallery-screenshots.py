#!/usr/bin/env python3
"""Capture the current gallery binary into the documentation asset set."""
from __future__ import annotations

import argparse
import re
import shutil
import subprocess
from pathlib import Path


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--binary", type=Path, required=True)
    parser.add_argument("--work", type=Path, required=True)
    parser.add_argument("--scale", type=float, default=1.0)
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    if not args.binary.is_absolute() or not args.work.is_absolute():
        parser.error("--binary and --work must be absolute")
    if not args.binary.is_file() or args.work.exists():
        parser.error("binary must exist and work must be fresh")
    capture = args.work / "capture"
    args.work.mkdir(parents=True)
    geometry = subprocess.check_output(["xwininfo", "-root"], text=True)
    dimensions = [re.search(r"^\s*" + key + r":\s*(\d+)", geometry, re.M)
                  for key in ("Width", "Height", "Depth")]
    if not all(dimensions):
        raise RuntimeError("Cannot determine the capture display geometry")
    viewport = "x".join(match.group(1) for match in dimensions)
    runner = root / "tests/automated/run-gallery-screenshots.py"
    subprocess.run([
        "python3", str(runner), "--binary", str(args.binary),
        "--out", str(capture), "--appearance", "modern-light", "--scale", str(args.scale),
    ], cwd=root, check=True)
    receipt = capture / "receipt.json"
    assets = root / "docs/gallery-screenshots"
    for png in capture.glob("*.png"):
        shutil.copy2(png, assets / png.name)
    subprocess.run([
        "python3", str(root / "tests/automated/generate-gallery-screenshot-manifest.py"),
        str(receipt), str(assets / "manifest.json"),
        "--viewport", viewport, "--profile", "modern-light",
        "--dpi-input", "server-default",
    ], cwd=root, check=True)
    subprocess.run(["python3", str(root / "tests/automated/check-gallery-screenshot-assets.py")],
                   cwd=root, check=True)
    print("Current modern-light gallery screenshots are ready for documentation")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
