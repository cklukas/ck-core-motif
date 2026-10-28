#!/usr/bin/env python3
"""Check that MWM IncludeCommand validates IPC message length."""
from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "clients" / "mwm" / "WmCmd.c").read_text()
start = source.index("IncludeCommand")
start = source.index("void", start)
end = source.index("\n}\n", start)
region = source[start:end]

assert "len < (unsigned long)(4 * sizeof(CARD32))" in region
assert "len - (unsigned long)(4 * sizeof(CARD32))" in region
assert "Truncated window list passed to IncludeCommand." in region
assert "UnpackCARD32(&data)" in region
print("MWM IncludeCommand length contract passed")
