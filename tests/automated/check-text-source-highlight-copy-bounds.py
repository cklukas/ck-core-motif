#!/usr/bin/env python3
"""Ensure shared Text-source highlight copies fit Xt's Cardinal size."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c").read_text(
    encoding="utf-8"
)
start = source.index("static void \nAddWidget")
end = source.index("\n/********************************<->***********************************/", start)
region = source[start:end]

assert "SIZE_MAX / sizeof(_XmHighlightRec)" in region
assert "(size_t)UINT_MAX / sizeof(_XmHighlightRec)" in region
assert "highlight_bytes = (size_t)data->widgets[0]->text.highlight.maximum" in region
assert "XtRealloc(" in region

print("text source highlight copy: size_t and Cardinal bounds checked")
