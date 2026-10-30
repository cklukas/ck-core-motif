#!/usr/bin/env python3
"""Check TabStack tear-off callback validates its widget."""

from pathlib import Path


source = Path("lib/Xm/TabStack.c").read_text(encoding="utf-8")
start = source.rindex("TearOffCallback(widget")
end = source.index("\n}", start)
body = source[start:end]
assert "if (widget == NULL)" in body
assert body.index("if (widget == NULL)") < body.index("XtVaGetValues(widget")

print("TabStack tear-off callback guard present")
