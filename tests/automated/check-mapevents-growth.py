#!/usr/bin/env python3
"""Check that key-event array growth is transactional on allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "MapEvents.c").read_text()
start = source.index("_XmMapKeyEvents(")
end = source.index("\n  /* Discard partial results if something fails. */", start)
region = source[start:end]

assert "int *new_event_types;" in region
assert "KeySym *new_keysyms;" in region
assert "Modifiers *new_modifiers;" in region
assert "if (count == INT_MAX)" in region
assert "if (new_event_types == NULL || new_keysyms == NULL ||" in region
assert "new_modifiers == NULL)" in region
assert "item_count > SIZE_MAX / sizeof(**eventTypes)" in region
assert "item_count > SIZE_MAX / sizeof(**keysyms)" in region
assert "item_count > SIZE_MAX / sizeof(**modifiers)" in region

print("PASS MapEvents array growth checks allocation failures")
