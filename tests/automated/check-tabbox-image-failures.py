#!/usr/bin/env python3
"""Check TabBox label drawing handles temporary X resource failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TabBox.c").read_text(
    encoding="utf-8"
)
start = source.index("data = (char*) XtMalloc")
end = source.index("return( dst );", start)
region = source[start:end]
assert "if (data == NULL)" in region
assert region.count("if (dst == NULL)") == 2
assert region.count("XtFree(data)") == 2

bitmap_guard = "if( !ValidPixmap(XmTabBox__bitmap(tab)) )\n\t\treturn;"
if source.count(bitmap_guard) != 2:
    raise SystemExit("TabBox's horizontal and vertical scratch bitmaps need guards")

rotate_guard = "if( dst_ximage == NULL )"
if source.count(rotate_guard) != 4:
    raise SystemExit("TabBox's horizontal and vertical rotated images need guards")

mask_guard = "if( pix != None )"
if source.count(mask_guard) != 2:
    raise SystemExit("TabBox's horizontal and vertical clip masks need guards")

fallback = "XPutImage(XtDisplay(tab), XiCanvas(tab), gc, dst_ximage, 0, 0,"
for offset in [i for i in range(len(source)) if source.startswith(mask_guard, i)]:
    fallback_offset = source.find(fallback, offset)
    if fallback_offset < offset:
        raise SystemExit("TabBox must draw directly when a clip-mask pixmap fails")

print("PASS TabBox label drawing handles temporary X resource failures")
