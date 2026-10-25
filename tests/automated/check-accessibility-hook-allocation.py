#!/usr/bin/env python3
"""Check that accessibility display-hook registration handles allocation failure."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "Accessible.c"
text = source.read_text(encoding="utf-8")
start = text.index("static void watch_display(")
end = text.index("\nstatic void disclosure_changed", start)
region = text[start:end]
assert "hook = (XmAccessibleDisplayHook *)XtMalloc" in region
assert "if (!hook)" in region
assert "return;" in region
print("accessibility display-hook registration guards allocation")
