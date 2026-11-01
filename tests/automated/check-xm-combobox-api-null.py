#!/usr/bin/env python3
from pathlib import Path
text=(Path(__file__).parents[2]/"lib"/"Xm"/"ComboBox.c").read_text()
assert text.count("if (widget == NULL)") >= 5
for name in ("XmComboBoxAddItem", "XmComboBoxDeletePos", "XmComboBoxSelectItem", "XmComboBoxSetItem", "XmComboBoxUpdate"):
    assert name in text
getter = text[text.index("XmCombinationBoxGetValue") :]
assert "XmString value = NULL;" in getter
assert "value = GetEditBoxValue(widget);" in getter
assert "return value;" in getter
print("XmComboBox APIs guard null widgets")
