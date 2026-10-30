#!/usr/bin/env python3
"""Check MenuShell layout-direction trait validates widget inputs."""

from pathlib import Path


source = Path("lib/Xm/MenuShell.c").read_text(encoding="utf-8")
start = source.rindex("GetDirection(Widget w)")
end = source.index("\n}", start)
body = source[start:end]
assert "w == NULL || !XtIsSubclass(w, xmMenuShellWidgetClass)" in body
assert "return XmDEFAULT_DIRECTION" in body

print("MenuShell direction guard present")
