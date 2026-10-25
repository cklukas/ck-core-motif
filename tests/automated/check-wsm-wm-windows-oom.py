#!/usr/bin/env python3
"""Check WSM WM_WINDOWS property unpacking handles allocation failure."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWsmLib/pack.c"
text = source.read_text(encoding="utf-8")
start = text.index("case WSM_WM_WINDOWS:", text.index("_WSMUnpackRequest"))
end = text.index("\n\tbreak;", start)
region = text[start:end]

assert "if (request->wm_windows.window_properties == NULL)" in region
assert "request->wm_windows.num_window_properties = 0;" in region
assert "num >= 0" in region
assert "UINT_MAX / sizeof(WindowProperty)" in region
assert "UINT_MAX / sizeof(AttributePair *)" in region
assert "XtMalloc((Cardinal)((size_t)num * sizeof(WindowProperty)))" in region
assert "XtMalloc((Cardinal)((size_t)num * sizeof(AttributePair *)))" in region
assert region.index("if (request->wm_windows.window_properties == NULL)") < region.index("for (i=0; i<num; i++)")
print("WSM WM_WINDOWS property unpacking handles allocation failure")
