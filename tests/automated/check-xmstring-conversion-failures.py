#!/usr/bin/env python3
"""Check XmString conversion constructors propagate representation failure."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
func = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringFunc.c").read_text()

assert "_XmStrCreate(str, XmSTRING_MULTIPLE_ENTRY, 0);\n    if (str == NULL) {\n      _XmStrContError(start) = TRUE;\n      return NULL;\n    }" in func
assert "_XmStrCreate(string, XmSTRING_OPTIMIZED, textlen);\n  if (string == NULL)" in source
assert "_XmStrCreate(string, XmSTRING_MULTIPLE_ENTRY, 0);\n  if (string == NULL)" in source

print("XmString conversion constructors propagate allocation failure")
