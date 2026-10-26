#!/usr/bin/env python3
"""Check that TextField XIM preedit positions are bounded."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c"
text = source.read_text(encoding="utf-8")
draw_start = text.index("static void\nPreeditDraw(")
draw_end = text.index("\n}\n", draw_start)
draw = text[draw_start:draw_end]
rendition_start = text.index("static void\nPreeditSetRendition(")
rendition_end = text.index("\n}\n", rendition_start)
rendition = text[rendition_start:rendition_end]

for item in ("call_data->caret < 0", "call_data->chg_first < 0",
             "call_data->chg_length < 0", "call_data->text->length < 0"):
    if item not in draw:
        raise SystemExit(f"missing TextField XIM guard: {item}")
if "text == NULL || text->length <= 0" not in rendition:
    raise SystemExit("TextField feedback helper lacks invalid-length guard")

print("TextField XIM preedit positions are bounded")
