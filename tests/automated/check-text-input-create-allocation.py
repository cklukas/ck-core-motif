#!/usr/bin/env python3
"""Check TextIn input-state creation handles partial allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextIn.c").read_text()
start = source.index("_XmTextInputCreate(")
body = source[start:start + 3200]
assert "tw->text.input = input = (Input) XtMalloc((Cardinal)sizeof(InputRec));" in body
assert "if (input == NULL)" in body
assert "input->data = data = (InputData) XtMalloc((Cardinal)sizeof(InputDataRec));" in body
assert "if (data == NULL)" in body
assert "tw->text.input = NULL;" in body
print("PASS TextIn input creation handles allocation failure")
