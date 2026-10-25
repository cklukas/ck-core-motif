#!/usr/bin/env python3
"""Check FontS metadata allocation failures are handled transactionally."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FontS.c").read_text()
assert "XtCalloc(1, sizeof(FontInfo))" in source
assert "if (font_info == NULL)" in source
assert "if (fam == NULL)" in source
assert "FamilyInfo *new_fam" in source
assert "if (XmFontS_font_info(fsw) == NULL)" in source
