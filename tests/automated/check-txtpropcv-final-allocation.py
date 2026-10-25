#!/usr/bin/env python3
"""Check final text-property buffer allocation handles failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c").read_text()
start = source.index("final_string =\n\t(unsigned char *)XtMalloc")
body = source[start:source.index("final_string[0]", start)]
assert "if (final_string == NULL)" in body
assert "XtFree(useable_text[i]);" in body
assert "XtFree((char *)useable_text);" in body
assert "_XmAppUnlock(app);" in body
assert "return BadAlloc;" in body
print("PASS final text-property buffer allocation is guarded")
