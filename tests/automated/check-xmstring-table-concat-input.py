#!/usr/bin/env python3
"""Check XmString table concatenation validates its input table."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringFunc.c").read_text()
start = source.index("XmStringTableToXmString(")
guard = source.index("if ((table == NULL) || (count == 0))", start)
loop = source.index("for (i = 0; i < count; i++)", guard)
assert guard < loop
assert "return(NULL);" in source[guard:loop]
print("PASS XmString table concatenation validates input")
