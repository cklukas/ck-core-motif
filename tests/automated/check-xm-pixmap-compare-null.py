#!/usr/bin/env python3
"""Check null-safe Xm pixmap cache comparison."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text(encoding="utf-8")
start = source.index("static Boolean\nComparePixmapDatas")
end = source.index("/* Hash a Pixmap entry", start)
body = source[start:end]
assert "data_1 != NULL && data_2 != NULL" in body
assert "data_1->acc_color != NULL && data_2->acc_color != NULL" in body
assert body.index("data_1->acc_color != NULL") < body.index("data_1->acc_color->foreground")
print("Xm pixmap comparison rejects null cache metadata")
