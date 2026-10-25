#!/usr/bin/env python3
"""Check WSM WM_WINDOWS match-attribute allocation failure handling."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWsmLib/pack.c"
text = source.read_text(encoding="utf-8")
start = text.index("case WSM_WM_WINDOWS:", text.index("_WSMUnpackRequest"))
end = text.index("\n\tbreak;", start)
region = text[start:end]

assert "if (request->wm_windows.match_attributes == NULL)" in region
assert "request->wm_windows.num_match_attributes = 0;" in region
assert region.index("if (request->wm_windows.match_attributes == NULL)") < region.index("for (i=0; i<request->wm_windows.num_match_attributes")
print("WSM match-attribute unpacking handles allocation failure")
