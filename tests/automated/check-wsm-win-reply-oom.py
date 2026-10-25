#!/usr/bin/env python3
"""Check WSM WM_WINDOWS reply unpacking handles allocation failure."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWsmLib/pack.c"
text = source.read_text(encoding="utf-8")
start = text.index("case WSM_WM_WINDOWS:", text.index("_WSMUnpackReply"))
end = text.index("\n\tbreak;", start)
region = text[start:end]

assert "if (reply->wm_windows.win_entry_list == NULL)" in region
assert "reply->wm_windows.num_win_entry_list = 0;" in region
assert "num >= 0" in region
assert "UINT_MAX / sizeof(WSMWinEntry)" in region
assert "XtMalloc((Cardinal)((size_t)num * sizeof(WSMWinEntry)))" in region
assert "XtMalloc(sizeof(WSMWinEntry) * num)" not in region
assert region.index("if (reply->wm_windows.win_entry_list == NULL)") < region.index("reply->wm_windows.allocated = True;")
print("WSM WM_WINDOWS reply unpacking handles allocation failure")
