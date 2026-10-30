#!/usr/bin/env python3
"""Check Scale's synthetic title getter validates callback inputs."""

from pathlib import Path


source = Path("lib/Xm/Scale.c").read_text(encoding="utf-8")
start = source.rindex("ScaleGetTitleString(")
end = source.index("\n}", start)
body = source[start:end]
assert "wid == NULL || value == NULL" in body
assert "XtIsSubclass(wid, xmScaleWidgetClass)" in body
assert "*value = (XtArgVal) NULL" in body

print("Scale title getter guard present")
