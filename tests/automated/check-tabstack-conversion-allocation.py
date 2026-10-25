#!/usr/bin/env python3
"""Check TabStack conversion refuses an unavailable child-value allocation."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TabStack.c").read_text()
start = source.index("TabConvertProc(")
end = source.index("\nstatic Atom\n", start)
region = source[start:end]

assert "passed_child = XtNew(Widget);" in region
assert "if (passed_child == NULL)" in region
assert region.index("if (passed_child == NULL)") < region.index("*passed_child = child")
assert "return(False);" in region
print("PASS TabStack conversion allocation failure is handled")
