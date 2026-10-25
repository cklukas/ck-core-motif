#!/usr/bin/env python3
"""Check null color-table handling in Mrm icon conversion."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmicon.c").read_text(encoding="utf-8")
start = source.index("UrmCreatePixmap (RGMIconImagePtr")
end = source.index("\nCardinal ", start + 1)
body = source[start:end]
assert "if ( icon->color_table.ctptr == NULL )" in body
assert "return MrmNOT_VALID;" in body
assert body.index("if ( icon->color_table.ctptr == NULL )") < body.index("Urm__RealizeColorTable")
start = source.index("Urm__RealizeColorTable (Screen")
body = source[start:]
assert "if ( ctable == NULL )" in body
assert body.index("if ( ctable == NULL )") < body.index("ctable->item")
print("Mrm icon conversion rejects null color tables")
