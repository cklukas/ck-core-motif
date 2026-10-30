#!/usr/bin/env python3
"""Check MessageBox synthetic getters validate widget classes."""

from pathlib import Path


source = Path("lib/Xm/MessageB.c").read_text(encoding="utf-8")
names = (
    "GetMessageString(", "GetSymbolPixmap(", "GetOkLabelString(",
    "GetCancelLabelString(", "GetHelpLabelString(",
)
for name in names:
    start = source.rindex(name)
    end = source.index("\n}", start)
    body = source[start:end]
    assert "!XtIsSubclass(wid, xmMessageBoxWidgetClass)" in body

print("MessageBox getter class guards present")
