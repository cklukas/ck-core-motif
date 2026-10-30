#!/usr/bin/env python3
"""Check CommandPalette rebuild does not gate entries on dead text allocation."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CommandPalette.c").read_text(
    encoding="utf-8"
)
start = source.index("static void rebuild(")
end = source.index("static void natural(", start)
region = source[start:end]

assert "make_display_string(" in region
assert "XmListAddItem" in region
assert "combined" not in region
assert "XtMalloc(length)" not in region
assert "has_shortcut = shortcut[0] != '\\0';" in region

print("PASS CommandPalette rebuild does not depend on unused combined text")
