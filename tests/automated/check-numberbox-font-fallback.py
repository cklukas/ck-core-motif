#!/usr/bin/env python3
"""NumberBox delegates text rendering to a native, owned-font editor."""
from pathlib import Path
source=Path("lib/Xm/NumberBox.c").read_text()
assert "xmTextFieldWidgetClass" in source and "XmNrenderTable" in source
assert "XmFontListCopy" in source and "XmRenderTableFree" in source
assert "if(new_table)" in source
assert "number->number_box.render_table=old->number_box.render_table" in source
assert "_XmAppearanceInheritFont" in source
assert "XmStringDraw" not in source
print("PASS NumberBox native editor and render-table ownership")
