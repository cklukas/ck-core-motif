#!/usr/bin/env python3
"""Check that MWM RemoveCommand validates its IPC payload length."""
from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "clients" / "mwm" / "WmCmd.c").read_text()
start = source.index("void\nRemoveCommand (")
end = source.index("\n}\n", start)
region = source[start:end]

assert "len < (unsigned long)(2 * sizeof(CARD32))" in region
assert "len - (unsigned long)(2 * sizeof(CARD32))" in region
assert "Truncated window list passed to RemoveCommand." in region
assert "UnpackCARD32(&data)" in region
print("MWM RemoveCommand length contract passed")
