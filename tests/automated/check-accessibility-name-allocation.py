#!/usr/bin/env python3
"""Check that accessibility creation handles its application-name allocation."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Accessible.c").read_text(encoding="utf-8")
start = source.index("XmAccessibilityCreate(Widget root")
end = source.index("Boolean XmAccessibilityAttachShell", start)
region = source[start:end]
assert "a->name = XtNewString" in region
assert "if (!a->name)" in region
assert "XmAccessibilityDestroy(a);" in region
assert "return NULL;" in region
print("PASS accessibility name allocation is guarded")
