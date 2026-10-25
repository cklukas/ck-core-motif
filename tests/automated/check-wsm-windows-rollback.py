#!/usr/bin/env python3
"""Ensure WSM window-list OOM paths release earlier allocations."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "pack.c").read_text(
    encoding="utf-8"
)
start = text.index("case WSM_WM_WINDOWS:", text.index("_WSMUnpackRequest"))
end = text.index("case WSM_WM_FOCUS:", start)
block = text[start:end]
assert block.count("XtFree((XtPointer)request->wm_windows.window_properties);") == 2
assert block.count("request->wm_windows.window_properties = NULL;") == 2
print("WSM window-list OOM paths release earlier allocations")
