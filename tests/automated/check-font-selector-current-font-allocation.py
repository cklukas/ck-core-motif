#!/usr/bin/env python3
"""Check that FontSelector current-font setup handles allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FontS.c").read_text(encoding="utf-8")
start = source.index("XmFontS_font_info(fsw)->current_font = (FontData *) XtMalloc")
end = source.index("/*\tFunction Name: Destroy", start)
region = source[start:end]
assert "if (XmFontS_font_info(fsw)->current_font == NULL)" in region
assert "SetDisplayedFont(fsw, XmFontS_current_font(fsw));" in region
print("PASS FontSelector current-font allocation is guarded")
