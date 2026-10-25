#!/usr/bin/env python3
"""Check XmStringFunc warning-backed guard paths."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "XmStringFunc.c"
text = source.read_text(encoding="utf-8")
assert "arg_list[i].value == 0" in text
assert "if (str == NULL) {" in text
assert "    type = XmeStringGetComponent(start, TRUE, FALSE, &len, &val);" in text
print("XmStringFunc argument and MakeStr guards are explicit")
