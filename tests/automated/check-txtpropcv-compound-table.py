#!/usr/bin/env python3
"""Check compound-text table conversion handles both output allocations."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c").read_text()
start = source.index("case XmSTYLE_COMPOUND_TEXT:")
body = source[start:source.index("case XmSTYLE_COMPOUND_STRING:", start)]
assert "compound_text = (char **)XtMalloc(sizeof(char *) * count);" in body
assert "if (compound_text == NULL)" in body
assert "xm_compound_text = (char *) XtMalloc(sizeof(char) * (total_size + 1));" in body
assert "if (xm_compound_text == NULL)" in body
assert "XtFree((char *)compound_text);" in body
assert "ptr = xm_compound_text;" in body
print("PASS compound-text table allocations are guarded")
