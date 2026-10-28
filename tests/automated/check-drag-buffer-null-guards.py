#!/usr/bin/env python3
"""Check DnD buffer helpers reject null payloads before memcpy."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragICC.c").read_text()
for name in ("_XmReadDragBuffer(", "_XmWriteDragBuffer("):
    start = source.index(name)
    end = source.index("/************************************************************************", start + len(name))
    body = source[start:end]
    assert "propBuf == NULL || (ptr == NULL && size != 0)" in body
    assert body.index("return 0") < body.index("memcpy")

print("PASS DnD buffer helpers guard null payloads")
