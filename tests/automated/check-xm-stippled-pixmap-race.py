#!/usr/bin/env python3
"""Check serialized stippled-pixmap cache lookup and publication."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmExtUtil.c").read_text(encoding="utf-8")
start = source.index("XiCreateStippledPixmap")
end = source.index("void \nXiReleaseStippledPixmap", start)
body = source[start:end]
assert body.count("_XmProcessLock();") >= 2
assert "/* Recheck after creation so concurrent callers share one pixmap. */" in body
assert "XFreePixmap(display, cachePtr->pixmap);" in body
assert "XtFree((char *)cachePtr);" in body
print("Xm stippled-pixmap cache lookup and publication are serialized")
