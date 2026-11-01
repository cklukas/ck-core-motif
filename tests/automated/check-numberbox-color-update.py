#!/usr/bin/env python3
"""Check the merged modern implementation contract; native suites exercise behavior."""
from pathlib import Path
source = (Path(__file__).parents[2] / "lib/Xm/NumberBox.c").read_text()
s = "".join(source.split())
assert "number->number_box.editor,number->number_box.increment,number->number_box.decrement,number->number_box.error_label" in s
assert "_XmAppearanceInheritForeground(color_children[i],widget);" in s
print("PASS numberbox-color-update")
