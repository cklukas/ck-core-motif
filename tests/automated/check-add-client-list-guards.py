#!/usr/bin/env python3
"""Check that AddClientToList validates its workspace and client arguments."""
from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWinList.c"
text = source.read_text()
start = text.index("void AddClientToList (WmWorkspaceData *pWS, ClientData *pCD, Boolean onTop)")
body = text[start:text.index("    Boolean belowSystemModal", start)]
assert "if (pWS == NULL || pCD == NULL)" in body
assert "return;" in body
print("AddClientToList validates workspace and client state")
