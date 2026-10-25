#!/usr/bin/env python3
"""Check XmAccessibilityCreate validates its root and allocation."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Accessible.c").read_text()
start = source.index("XmAccessibility *XmAccessibilityCreate(")
end = source.index("\n}\n", start) + 3
region = source[start:end]
assert "if (!root)" in region
assert "if (!a)" in region
assert region.index("if (!root)") < region.index("XtWidgetToApplicationContext")
assert region.index("if (!a)") < region.index("a->root")
print("PASS accessibility creation input validation")
