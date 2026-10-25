#!/usr/bin/env python3
"""Check that Picture subgraph bookkeeping handles allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Picture.c").read_text(encoding="utf-8")
start = source.index("static XmPictureNode*\n_XmPictureCopySubGraph")
end = source.index("static void\n_XmPictureFillTraverse", start)
region = source[start:end]
assert "XtMalloc(tablesize * sizeof(XmPictureNode*)" in region
assert "if (table == NULL)" in region
assert "return end;" in region
print("PASS Picture subgraph allocation is guarded")
