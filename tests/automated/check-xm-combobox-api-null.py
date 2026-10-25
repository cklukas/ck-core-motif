#!/usr/bin/env python3
from pathlib import Path
text=(Path(__file__).parents[2]/"lib"/"Xm"/"ComboBox.c").read_text()
assert text.count("if (widget == NULL)") >= 5
for name in ("XmComboBoxAddItem", "XmComboBoxDeletePos", "XmComboBoxSelectItem", "XmComboBoxSetItem", "XmComboBoxUpdate"):
    assert name in text
assert "return (GetEditBoxValue(widget));" in text
print("XmComboBox APIs guard null widgets")
