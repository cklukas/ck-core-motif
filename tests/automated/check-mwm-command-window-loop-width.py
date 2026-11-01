#!/usr/bin/env python3
"""Check WSM command window-list loops retain the protocol count width."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmCmd.c").read_text(
    encoding="utf-8"
)

for function in ("IncludeCommand", "EnableCommand", "DisableCommand",
                 "RenameCommand", "RemoveCommand"):
    start = source.index(function + " (\n")
    end = source.index("\n}\n", start) + 2
    body = source[start:end]
    assert "CARD32         i, win;" in body, function
    assert "int            i, win;" not in body, function
    assert "for (win=0; win<count; win++)" in body, function
    assert "for (i=0; i<count; i++)" in body, function

print("PASS MWM command window-list loops preserve CARD32 count width")
