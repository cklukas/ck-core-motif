#!/usr/bin/env python3
"""Check safe toolbar action-label composition."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Toolbar.c").read_text(encoding="utf-8")
start = source.index("static XmString action_label")
end = source.index("static void free_actions", start)
region = source[start:end]

assert "if (!separator || !shortcut)" in region
assert "combined = XmStringConcat(result, separator)" in region
assert "result = XmStringConcat(combined, shortcut)" in region
assert "XmStringFree(separator)" in region
assert "XmStringFree(shortcut)" in region
print("PASS toolbar action labels validate XmString composition and cleanup")
