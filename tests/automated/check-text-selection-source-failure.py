#!/usr/bin/env python3
"""Ensure text selection encoders handle source-string failure."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "TextSel.c"
text = source.read_text(encoding="utf-8")
assert text.count("tmp_value = _XmStringSourceGetString") == 1
assert text.count("tmp_value =_XmStringSourceGetString") == 2
assert text.count("if (tmp_value == NULL)") >= 3
assert text.count("*length = 0;") >= 3

print("text selection source-string failure guards: ok")
