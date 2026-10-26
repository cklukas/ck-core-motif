#!/usr/bin/env python3
"""Check shell title/callback helpers validate widgets before context use."""
from pathlib import Path

xm_string = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text(
    encoding="utf-8"
)
vendor = (Path(__file__).parents[2] / "lib" / "Xm" / "VendorSE.c").read_text(
    encoding="utf-8"
)

cases = (
    (xm_string, "XmeSetWMShellTitle(", "if (shell == (Widget) NULL)\n    return;", "_XmWidgetToAppContext(shell)"),
    (vendor, "XmeAddFocusChangeCallback(Widget w,", "if (w == (Widget) NULL)\n    return;", "_XmWidgetToAppContext(w)"),
    (vendor, "XmeRemoveFocusChangeCallback(Widget w,", "if (w == (Widget) NULL)\n    return;", "_XmWidgetToAppContext(w)"),
)
for source, signature, guard, lookup in cases:
    start = source.index(signature)
    body_start = source.index("{", start)
    body_end = source.index("\n}", body_start)
    body = source[body_start:body_end]
    assert guard in body
    assert body.index(guard) < body.index(lookup)

print("shell title and focus callbacks guard null widgets before context lookup")
