#!/usr/bin/env python3
"""Check the merged modern implementation contract; native suites exercise behavior."""
from pathlib import Path
source = (Path(__file__).parents[2] / "lib/Xm/NumberBox.c").read_text()
s = "".join(source.split())
assert "xmTextFieldWidgetClass" in s
assert "XtConfigureWidget(number->number_box.editor,editor_x,0,editor_width,field_height,0)" in s
assert "inteditor_width=MAX(1,width-arrow_width-gap)" in s
assert "XmeDrawHighlight" not in s
print("PASS number-box-highlight-bounds")
