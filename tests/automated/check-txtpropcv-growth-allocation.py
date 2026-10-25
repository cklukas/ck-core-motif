#!/usr/bin/env python3
"""Check locale text assembly handles final-string growth failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c").read_text()
start = source.index("GetUseableText(")
body = source[start:source.index("/* If we encountered an invalid segment", start)]
assert "new_final_string = (char *)XtRealloc(final_string, size_so_far + 1);" in body
assert "if (new_final_string == NULL)" in body
assert "XtFree(text);" in body
assert "XtFree(final_string);" in body
assert "_XmStringContextFree(&stack_context);" in body
assert "return(XNoMemory);" in body
assert "final_string = new_final_string;" in body
print("PASS text assembly growth allocation is guarded")
