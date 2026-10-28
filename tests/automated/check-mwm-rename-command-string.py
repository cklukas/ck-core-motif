#!/usr/bin/env python3
"""Check that MWM RenameCommand bounds its name and window list."""
from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "clients" / "mwm" / "WmCmd.c").read_text()
start = source.index("void\nRenameCommand (")
end = source.index("/*----------------------------------------------------------------------*\n |                             RemoveCommand", start)
region = source[start:end]

assert "UnpackCommandString(&data, &remaining)" in region
assert "remaining < sizeof(CARD32)" in region
assert "remaining / sizeof(CARD32)" in region
assert "Truncated string data passed to RenameCommand." in region
assert "Truncated window list passed to RenameCommand." in region
assert "UnpackString(&data)" not in region
assert "XtFree(newname);" in region
print("MWM RenameCommand string contract passed")
