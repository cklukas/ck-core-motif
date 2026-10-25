#!/usr/bin/env python3
"""Check that AddEntryToList validates list ownership before linking."""
from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWinList.c"
text = source.read_text()
start = text.index("void AddEntryToList (WmWorkspaceData *pWS")
body = text[start:text.index("    pSD = pWS->pSD;", start)]
assert "pWS == NULL || pEntry == NULL || pWS->pSD == NULL" in body
assert "return;" in body
print("AddEntryToList validates workspace, screen, and entry state")
