#!/usr/bin/env python3
"""Ensure XmInfoBar draws through its render table."""
from pathlib import Path

root = Path(__file__).parents[2]
part = (root / "lib/Xm/InfoBarP.h").read_text(encoding="utf-8")
source = (root / "lib/Xm/InfoBar.c").read_text(encoding="utf-8")
assert "XmRenderTable font" in part
assert "XmNrenderTable" in source
assert "XmStringExtent" in source
assert "XmStringDraw" in source
assert "XmRenderTableFree" in source
assert "if(widget==NULL||!XmIsInfoBar(widget))return;" in source
assert "if(widget!=NULL&&XmIsInfoBar(widget))" in source
print("PASS InfoBar text uses its render table")
