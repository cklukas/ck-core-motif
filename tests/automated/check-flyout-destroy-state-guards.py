#!/usr/bin/env python3
"""Check Flyout destroy callbacks avoid mutating a tearing-down widget."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Flyout.c").read_text(
    encoding="utf-8"
)
for name in ("static void owner_destroyed(Widget owner", "static void popup_destroyed(Widget shell"):
    start = source.index(name)
    end = source.index("\n}", start) + 2
    body = source[start:end]
    assert "!XmIsFlyout((Widget)flyout)" in body
    assert "flyout->core.being_destroyed" in body

print("PASS Flyout destroy callbacks guard tearing-down widgets")
