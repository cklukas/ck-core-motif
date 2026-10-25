#!/usr/bin/env python3
"""Check internal XmList node allocation failure handling."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "xmlist.c").read_text()
start = source.index("\n_Xm_GetNewElement(") + 1
end = source.index("\n}\n", start) + 3
allocator = source[start:end]
assert "new_elems = (_XmQElem *) XtCalloc" in allocator
assert "if (new_elems == NULL)" in allocator
assert "return(NULL);" in allocator

start = source.index("\n_XmListAddAfter(") + 1
end = source.index("\n}\n", start) + 3
adder = source[start:end]
assert "if (elem == NULL)" in adder
assert adder.index("if (elem == NULL)") < adder.index("_Xm_AddQueue")
print("PASS internal XmList allocation failure handling")
