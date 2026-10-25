#!/usr/bin/env python3
"""Check that DeleteEntryFromList validates list ownership."""
from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWinList.c"
text = source.read_text()
start = text.index("void DeleteEntryFromList (WmWorkspaceData *pWS")
body = text[start:text.index("    if (pListEntry->prevSibling)", start)]
assert "pWS == NULL || pListEntry == NULL || pWS->pSD == NULL" in body
assert "return;" in body
print("DeleteEntryFromList validates workspace, screen, and entry state")
