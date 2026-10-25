#!/usr/bin/env python3
"""Run the native gallery integration test when a display is available."""
from __future__ import annotations

import os
import subprocess
import sys
from pathlib import Path


def main() -> int:
    root = Path(__file__).resolve().parents[2]
    binary = Path(os.environ.get("CKMOTIF_GALLERY", root / "demos/programs/ckmotif-gallery/.libs/ckmotif-gallery"))
    if not binary.exists():
        print(f"skip: gallery binary not found: {binary}")
        return 77
    if not os.environ.get("DISPLAY"):
        print("skip: DISPLAY is not set")
        return 77
    result = subprocess.run([str(binary), "--integration-test"],
                            cwd=root, env=os.environ.copy(), timeout=10,
                            capture_output=True, text=True)
    sys.stdout.write(result.stdout)
    sys.stderr.write(result.stderr)
    if (result.returncode == 0 and
            "navigation tabs interaction state PASS" in result.stdout and
            "keyboard traversal and focus restoration PASS" in result.stdout and
            "acceptance metadata" in result.stdout and
            "theme/dpi live transaction PASS" in result.stdout):
        return 0
    return result.returncode or 1


if __name__ == "__main__":
    raise SystemExit(main())
