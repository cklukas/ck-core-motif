#!/usr/bin/env python3
"""Check ComboBox synthetic getters return deterministic pre-init defaults."""

from pathlib import Path


source = Path("lib/Xm/ComboBox.c").read_text()
start = source.index("CBGetSelectedItem(Widget")
end = source.index("/*\n * CBSetSelectedPos", start)
item_body = source[start:end]
assert "CB_EditBox((XmComboBoxWidget)widget) == NULL" in item_body
assert "*value = (XtArgVal)NULL;" in item_body

start = source.index("CBGetSelectedPos(Widget")
end = source.index("/*\n * CBGetColumns", start)
pos_body = source[start:end]
assert "*value = (XtArgVal)XmINVALID_POSITION;" in pos_body
assert "CB_List(cb) == NULL" in pos_body

set_pos = source[source.index("CBSetSelectedPos(Widget"):start]
assert "!cb->composite.num_children || CB_List(cb) == NULL" in set_pos

print("ComboBox synthetic getters provide deterministic defaults")
