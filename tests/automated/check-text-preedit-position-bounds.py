#!/usr/bin/env python3
"""Check that XIM preedit positions are validated before text arithmetic."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Text.c"
text = source.read_text(encoding="utf-8")
start = text.index("static void\nPreeditDraw(")
end = text.index("\n}\n", start)
region = text[start:end]

required = (
    "call_data->caret < 0",
    "call_data->chg_first < 0",
    "call_data->chg_length < 0",
)
if any(item not in region for item in required):
    raise SystemExit("negative XIM preedit positions are not rejected")
if region.index("call_data->caret < 0") > region.index("XmTextSetHighlight"):
    raise SystemExit("preedit positions are checked too late")

print("XIM preedit positions are bounded before arithmetic")
