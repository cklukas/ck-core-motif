#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/Accessible.c").read_text()

assert "static int bounded_cardinal(Cardinal value)" in source
assert "value > (Cardinal)INT_MAX ? INT_MAX : (int)value" in source
assert "static int bounded_text_position(XmTextPosition value)" in source
assert "value > (XmTextPosition)INT_MAX ? INT_MAX : (int)value" in source
assert "i->rows = bounded_cardinal(XmDataGridGetRowCount(w));" in source
assert "i->columns = bounded_cardinal(XmDataGridGetColumnCount(w));" in source
assert "i->text_length = bounded_text_position" in source
assert "i->caret = bounded_text_position" in source
