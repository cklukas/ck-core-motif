#!/usr/bin/env python3
"""Check TextField activation lifetime protection around callbacks."""

from pathlib import Path


source = Path("lib/Xm/TextF.c").read_text()
start = source.index("\nActivate(Widget w,")
body = source[start:source.index("static void\nSetAnchorBalancing", start)]

guard = "if (!_XmCallCallbacksUntilDestroyed(w, XmNactivateCallback, (XtPointer)&cb)) return;"
assert guard in body
assert body.index(guard) < body.index("_XmParentProcess(XtParent(w)")

print("TextField activation lifetime guard is present")
