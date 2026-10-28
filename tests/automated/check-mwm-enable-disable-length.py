#!/usr/bin/env python3
"""Check that MWM enable/disable IPC handlers validate payload lengths."""
from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "clients" / "mwm" / "WmCmd.c").read_text()
for name in ("EnableCommand", "DisableCommand"):
    start = source.index(name)
    start = source.index("void", start)
    end = source.index("\n}\n", start)
    region = source[start:end]
    assert "len < (unsigned long)(2 * sizeof(CARD32))" in region
    assert "len - (unsigned long)(2 * sizeof(CARD32))" in region
    assert "Truncated window list passed to" in region
    assert "UnpackCARD32(&data)" in region
print("MWM enable/disable length contract passed")
