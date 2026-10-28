#!/usr/bin/env python3
"""Check DropDown frees unparse text when multibyte conversion fails."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropDown.c").read_text()
start = source.index("len = mbstowcs")
end = source.index("\n\tXmTextFieldInsert", start)
body = source[start:end]

assert "if (len == (size_t)(-1))" in body
assert "XtFree((char *) ptr);" in body
assert body.index("XtFree((char *) ptr);") < body.index("continue;")

print("DropDown frees unparse text after multibyte conversion failure")
