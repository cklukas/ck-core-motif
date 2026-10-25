#!/usr/bin/env python3
"""Check SimpleSpinBox value-list mutations are transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "SSpinB.c").read_text()

add_start = source.index("XmSimpleSpinBoxAddItem(")
add_end = source.index("\n}\n", add_start) + 2
add = source[add_start:add_end]
assert "UINT_MAX - sizeof(XmString)" in add
assert "new_position = ssb_p->position;" in add
assert "goto add_copy_failure;" in add
assert "while (i != 0)" in add
assert "XmStringFree(values[--i])" in add
assert "XmNposition, new_position" in add

delete_start = source.index("XmSimpleSpinBoxDeletePos(")
delete_end = source.index("\n}\n", delete_start) + 2
delete = source[delete_start:delete_end]
assert "UINT_MAX / sizeof(XmString)" in delete
assert "new_position = ssb_p->position;" in delete
assert "goto delete_copy_failure;" in delete
assert "int\t\t\ti, copied;" in delete
assert "while (copied != 0)" in delete
assert "XmStringFree(values[--copied])" in delete
assert "XmNposition, new_position" in delete
print("SimpleSpinBox value-list mutations preserve state on copy failure")
