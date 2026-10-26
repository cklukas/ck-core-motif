#!/usr/bin/env python3
"""Check wide Text replacement handles its temporary buffer allocation."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextFunc.c").read_text()
start = source.index("void\n_XmTextReplace")
end = source.index("\nvoid\nXmTextReplace", start)
body = source[start:end]

assert "tmp_block = XtMalloc" in body
assert "num_chars + 1 > (size_t)UINT_MAX / (size_t)tw->text.char_size" in body
assert "conversion_bytes =" in body
assert "if (tmp_block == NULL)" in body
assert "DrawInsertionPoint)(tw,\n\t\t\t\t\t      tw->text.cursor_position, on);" in body
assert "return;" in body

print("Wide Text replacement checks temporary-buffer allocation")
