#!/usr/bin/env python3
"""Check TextField activation lifetime protection around callbacks."""

from pathlib import Path


source = Path("lib/Xm/TextF.c").read_text()
start = source.index("ActivationDestroyed(Widget widget")
body = source[start:source.index("static void\nSetAnchorBalancing", start)]

assert "XtAddCallback(w, XmNdestroyCallback, ActivationDestroyed" in body
assert "XtCallCallbackList(w, TextF_ActivateCallback(tf)" in body
assert "if (destroyed) return;" in body
assert "XtRemoveCallback(w, XmNdestroyCallback, ActivationDestroyed" in body
assert "w->core.being_destroyed" in body

print("TextField activation lifetime guard is present")
