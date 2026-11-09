#!/usr/bin/env python3
"""Check icon-box callbacks validate their X event types before use."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmIconBox.c"
text = source.read_text(encoding="utf-8")

assert "void HandleIconBoxIconKeyPress" in text
assert "if (icon == NULL || event == NULL || event->type != KeyPress)" in text
focus_start = text.index("void ChangeActiveIconboxIconText")
focus_end = text.index("void HandleIconBoxIconKeyPress", focus_start)
focus = text[focus_start:focus_end]
assert "(event->type != FocusIn && event->type != FocusOut)" in focus
print("icon-box callbacks validate X event types before union access")
