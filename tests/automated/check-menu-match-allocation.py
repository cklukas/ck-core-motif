#!/usr/bin/env python3
"""Check menu-match insertion handles allocation failures atomically."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmMenu.c"
text = source.read_text(encoding="utf-8")

needle = "new = (MatchList *)XtMalloc(sizeof(MatchList));"
assert needle in text
guard = text[text.index(needle):text.index(needle) + 1000]
assert "if (new == (MatchList *) NULL)" in guard
assert "new->command_string == (String) NULL" in guard
assert "new->funcargs == (String) NULL" in guard
assert "XtFree ((char *) new->command_string);" in guard
print("menu-match insertion handles allocation failures")
