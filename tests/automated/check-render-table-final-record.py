#!/usr/bin/env python3
"""Ensure the render-table importer retains a newline-less final record."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text(
    encoding="utf-8"
)
start = source.index("XmRenderTableCvtFromProp(")
end = source.index("\nvoid\nXmRenderTableGetDefaultFontExtents", start)
body = source[start:end]

append = "rarray[rarray_count] = rendition;\n      rarray_count++;"
eof = "if (token -> type == T_EOF) goto finish;"
assert body.index(append) < body.index(eof, body.index(append))

growth_start = body.index("if (rarray_count >= rarray_max)")
growth_end = body.index("rarray[rarray_count]", growth_start)
growth = body[growth_start:growth_end]
assert growth.count("XmRenditionFree(rendition);") == 2
assert "new_array == NULL) {" in growth

print("PASS render-table import retains an EOF-terminated final rendition")
