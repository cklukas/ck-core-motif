#!/usr/bin/env python3
"""Check XmList top-position conversion callbacks validate their inputs."""

from pathlib import Path


source = Path("lib/Xm/List.c").read_text()
start = source.rindex("CvtToExternalPos(Widget wid,")
end = source.index("\n}\n", start) + 2
external = source[start:end]
assert "value == NULL" in external
assert "wid == NULL" in external
assert "XtIsSubclass(wid, xmListWidgetClass)" in external
assert "XmINVALID_POSITION" in external

start = source.rindex("CvtToInternalPos(Widget wid,")
end = source.index("\n}\n", start) + 2
internal = source[start:end]
assert "value == NULL" in internal
assert "XmSYNTHETIC_NONE" in internal

print("XmList top-position conversions validate callback inputs")
