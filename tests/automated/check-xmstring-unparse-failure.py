#!/usr/bin/env python3
"""Check XmStringUnparse reports helper allocation/conversion failures."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
helper_start = source.index("static void\nunparse_text(")
helper_end = source.index("/*\n * unparse_is_plausible", helper_start)
helper = source[helper_start:helper_end]
assert "#define UNPARSE_FAIL()" in helper
assert "*length = -1" in helper
public_start = source.index("XtPointer\nXmStringUnparse(")
public_end = source.index("XmString\nXmStringComponentCreate(", public_start)
public = source[public_start:public_end]
assert "if (length < 0)" in public
assert "XtFree(result);" in public
assert "return NULL;" in public
print("PASS XmStringUnparse failure propagation")
