#!/usr/bin/env python3
"""Check XmString table unparsing handles output allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringFunc.c").read_text()
marker = "XmStringTableUnparse("
start_func = source.index(marker)
allocation = source.index("strs = (XtPointer *)XtMalloc(count * sizeof(XtPointer));", start_func)
guard = source.index("if (strs == NULL)", allocation)
write = source.index("strs[i] = XmStringUnparse", guard)
assert allocation < guard < write
assert "_XmProcessUnlock();" in source[guard:write]
assert "return(NULL);" in source[guard:write]
print("PASS XmString table unparse allocation failure is handled")
