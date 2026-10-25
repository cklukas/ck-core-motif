#!/usr/bin/env python3
"""Check WSM WM_WINDOWS per-match allocation rollback."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWsmLib/pack.c"
text = source.read_text(encoding="utf-8")
start = text.index("case WSM_WM_WINDOWS:", text.index("_WSMUnpackRequest"))
end = text.index("\n\tbreak;", start)
region = text[start:end]

assert "if (request->wm_windows.match_attributes[i] == NULL)" in region
assert "XtFree((XtPointer)request->wm_windows.match_attributes[j]);" in region
assert "request->wm_windows.match_attributes = NULL;" in region
assert "request->wm_windows.num_match_attributes = 0;" in region
print("WSM per-match allocation rolls back safely")
