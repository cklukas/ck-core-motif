#!/usr/bin/env python3
"""Check menu recreation validates its temporary pointer array."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmMenu.c"
text = source.read_text(encoding="utf-8")

needle = "to_change = (MenuSpec **)XtMalloc(sizeof(MenuSpec *) * count);"
assert needle in text
guard = text[text.index(needle):text.index(needle) + 220]
assert "if (to_change == (MenuSpec **) NULL)" in guard
assert "return;" in guard
print("menu recreation checks its temporary allocation")
