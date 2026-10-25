#!/usr/bin/env python3
"""Check WSM set-state unpacking fails closed on array allocation failure."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWsmLib/pack.c"
text = source.read_text(encoding="utf-8")
start = text.index("case WSM_SET_STATE:", text.index("_WSMUnpackRequest"))
end = text.index("\n\t}\n\tbreak;", start)
region = text[start:end]

assert "if (request->set_state.win_info_list == NULL)" in region
assert "request->set_state.num_win_info_list = 0;" in region
assert "request->set_state.allocated = True;" in region
assert "num >= 0" in region
assert "UINT_MAX / sizeof(WSMWinInfo)" in region
assert "XtMalloc((Cardinal)((size_t)num * sizeof(WSMWinInfo)))" in region
assert "XtMalloc(sizeof(WSMWinInfo) * num)" not in region
assert "request->extensions.allocated = True;" not in region
print("WSM set-state unpacking handles allocation failure")
