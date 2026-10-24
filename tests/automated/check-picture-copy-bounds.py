#!/usr/bin/env python3
"""Check bounded node-map sizing for XmPicture repeat expansion."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Picture.c"
text = source.read_text(encoding="utf-8")
start = text.index("static XmPictureNode*\n_XmPictureCopySubGraph(")
end = text.index("\nstatic void", start)
region = text[start:end]

for needle in (
    "tablesize <= 0",
    "SIZE_MAX / sizeof(XmPictureNode *)",
    "return end;",
):
    if needle not in region:
        raise SystemExit(f"picture node-map capacity guard missing: {needle}")

print("XmPicture repeat node-map sizing is bounded")
