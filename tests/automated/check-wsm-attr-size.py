#!/usr/bin/env python3
"""Check WSM unsupported attribute sizes produce an inert record."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWsmLib/pack.c"
text = source.read_text(encoding="utf-8")
start = text.index("UnpackSingleWinDataRec(", text.index("Function Name: UnpackSingleWinDataRec"))
end = text.index("\n}\n", start) + 2
region = text[start:end]

assert "default:" in region
assert "win_data->type = WSM_NONE;" in region
assert "win_data->data_len = 0;" in region
print("WSM unsupported attribute sizes fail closed")
