#!/usr/bin/env python3
"""Ensure WSM window requests mark the correct ownership field."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "pack.c").read_text(
    encoding="utf-8"
)
start = text.index("case WSM_WM_WINDOWS:", text.index("_WSMUnpackRequest"))
end = text.index("case WSM_WM_FOCUS:", start)
block = text[start:end]
assert "request->wm_windows.allocated = True;" in block
assert "request->extensions.allocated = True;" not in block
print("WSM window requests mark their own allocations")
