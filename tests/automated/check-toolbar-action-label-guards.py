#!/usr/bin/env python3
"""Check safe toolbar action-label composition."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Toolbar.c").read_text(encoding="utf-8")
start = source.index("static XmString action_label")
end = source.index("static void free_actions", start)
region = source[start:end]

assert "if (!label || !shortcut)" in region
assert "label_length > SIZE_MAX - 3" in region
assert "shortcut_length > SIZE_MAX - 3 - label_length" in region
assert "if (!text)" in region
copy_start = source.index("static XmString string_copy")
copy_end = source.index("static XmString action_label", copy_start)
assert "if (!text)" in source[copy_start:copy_end]
print("PASS toolbar action labels validate conversion and allocation")
