#!/usr/bin/env python3
"""Check null color-table handling in Mrm icon conversion."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmicon.c").read_text(encoding="utf-8")
start = source.index("UrmCreatePixmap (RGMIconImagePtr")
end = source.index("\nCardinal ", start + 1)
body = source[start:end]
guard = body.index("if ( icon->color_table.ctptr == NULL )")
if "return MrmNOT_VALID;" not in body[guard:guard + 120]:
    raise SystemExit("UrmCreatePixmap does not reject null color tables")
if guard > body.index("Urm__RealizeColorTable"):
    raise SystemExit("color-table validation occurs after realization")
start = source.index("Urm__RealizeColorTable (Screen")
body = source[start:]
guard = body.index("if ( ctable == NULL || ctable->count < URMColorTableUserMin )")
if "return MrmNOT_VALID;" not in body[guard:guard + 120] or guard > body.index("ctable->item"):
    raise SystemExit("color-table realization is not null-safe")
print("PASS Mrm icon conversion rejects null color tables")
