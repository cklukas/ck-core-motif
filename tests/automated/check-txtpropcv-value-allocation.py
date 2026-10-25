#!/usr/bin/env python3
"""Check fallback text-property value allocation handles failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c").read_text()
start = source.index("txt_value = XtMalloc")
body = source[start:source.index("strcpy(txt_value", start)]
assert "if (txt_value == NULL)" in body
assert "XtFree(compound_text);" in body
assert "_XmStringContextFree(&stack_context);" in body
assert "return XNoMemory;" in body
print("PASS fallback text-property value allocation is guarded")
