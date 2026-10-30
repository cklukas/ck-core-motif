#!/usr/bin/env python3
"""Check empty plain text import reports XmString generation failure."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c").read_text()
start = source.index("if (text_prop->nitems == 0)")
end = source.index("    /* We fell through", start)
body = source[start:end]

assert 'string_table[0] = XmStringGenerate((XtPointer)"", tag, type, NULL);' in body
assert "if (string_table[0] == NULL)" in body
assert "XtFree((char *)string_table);" in body
assert "return BadAlloc;" in body

print("empty plain text import checks XmString generation")
