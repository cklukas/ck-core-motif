#!/usr/bin/env python3
"""Check internal XmList stack growth failure handling."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "xmlist.c").read_text()
start = source.index("\n_XmStackPush(") + 1
end = source.index("\n}\n", start) + 3
region = source[start:end]
assert "if (stack == NULL)" in region
assert "new_elems = (XtPointer *) XtRealloc" in region
assert "if (new_elems == NULL)" in region
assert region.index("if (new_elems == NULL)") < region.index("stack->elems = new_elems")
assert "stack->elems[++stack->top] = elem;" in region
assert "(Cardinal)(sizeof(XtPointer) *" in region

start = source.index("\n_XmStackPop(") + 1
end = source.index("\n}\n", start) + 3
assert "stack == NULL" in source[start:end]
print("PASS internal XmList stack allocation failure handling")
