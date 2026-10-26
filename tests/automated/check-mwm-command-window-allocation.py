#!/usr/bin/env python3
"""Check that MWM command window lists reject empty and failed allocations."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmCmd.c").read_text(encoding="utf-8")
for command in ("IncludeCommand", "EnableCommand", "DisableCommand", "RenameCommand", "RemoveCommand"):
    start = source.index(command + " (")
    end = source.find("\n/*----------------------------------------------------------------------*", start + 1)
    if end == -1:
        end = len(source)
    region = source[start:end]
    assert "if (count == 0)" in region, command
    assert "windowIDs = (Window *) XtMalloc((Cardinal)(sizeof(Window) * (size_t) count));" in region, command
    assert "(size_t) count > SIZE_MAX / sizeof(Window)" in region, command
    assert "if (windowIDs == NULL)" in region, command
print("PASS MWM command window-list inputs are guarded")
