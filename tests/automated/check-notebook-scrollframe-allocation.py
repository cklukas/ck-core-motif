#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/Notebook.c").read_text()
start = source.index("ScrollFrameInit (")
end = source.index("ScrollFrameGetInfo (", start)
region = source[start:end]
assert "XtMalloc(sizeof(XmScrollFrameDataRec))" in region
assert "if (nb->notebook.scroll_frame_data == NULL)" in region
assert "return;" in region
print("Notebook scroll-frame allocation guard present")
