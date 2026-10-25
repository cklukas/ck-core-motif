#!/usr/bin/env python3
"""Ensure MWM command window-list allocations cannot wrap."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmCmd.c").read_text(
    encoding="utf-8"
)
handlers = ("IncludeCommand", "EnableCommand", "DisableCommand", "RenameCommand", "RemoveCommand")
for handler in handlers:
    start = text.index(handler + " (")
    end = text.index("\n}\n", start)
    function = text[start:end]
    assert "(size_t) count > ((size_t) -1) / sizeof(Window)" in function, handler
    assert "sizeof(Window) * (size_t) count" in function, handler
print("MWM window-list allocations guard multiplication")
