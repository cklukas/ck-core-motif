#!/usr/bin/env python3
"""Check that StackWindow validates list state before X restacking."""
from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWinList.c"
text = source.read_text()
start = text.index("void StackWindow (WmWorkspaceData *pWS")
body = text[start:text.index("    Window stackWindow;", start)]
assert "pWS == NULL || pWS->pSD == NULL" in body
assert "pEntry == NULL" in body
assert "pEntry->pCD == NULL" in body
assert "pStackEntry != NULL && pStackEntry->pCD == NULL" in body
restack = text[start:text.index("} /* END OF FUNCTION StackWindow */", start)]
assert "!onTop && pSD->lastClient == NULL" in restack
print("StackWindow validates workspace and list entries")
