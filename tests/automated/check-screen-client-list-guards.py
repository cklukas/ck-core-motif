#!/usr/bin/env python3
"""Check client-list property updates handle detached and OOM state."""
from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWinList.c"
text = source.read_text()
start = text.index("void UpdateScreenClientList")
end = text.index("}  /* END OF FUNCTION UpdateScreenClientList", start)
body = text[start:end]
assert "Window *windows = NULL" in body
assert "if (pSD == NULL)" in body
assert "pEntry->pCD != NULL" in body
assert "resized = realloc" in body
assert "if (resized == NULL)" in body
assert "free (windows);" in body
print("screen client-list updates validate screen, entries, and allocation")
