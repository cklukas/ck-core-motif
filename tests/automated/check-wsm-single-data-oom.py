#!/usr/bin/env python3
"""Check WSM per-attribute list allocations before filling them."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWsmLib/pack.c"
text = source.read_text(encoding="utf-8")
start = text.index("UnpackSingleWinDataRec(", text.index("Function Name: UnpackSingleWinDataRec"))
end = text.index("\nstatic void\nSkipWinDataRec", start)
region = text[start:end]

assert region.count("if (local == NULL && win_data->data_len != 0)") == 3
assert region.count("win_data->type = WSM_NONE;") >= 4
assert region.count("win_data->data_len = 0;") >= 4
print("WSM per-attribute list allocations are guarded")
