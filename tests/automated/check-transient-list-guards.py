#!/usr/bin/env python3
"""Check transient list helpers validate detached tree state."""
from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWinList.c"
text = source.read_text()
add = text.index("void AddTransient (WmWorkspaceData *pWS, ClientData *pCD)")
add_body = text[add:text.index("    pcdLeader = pCD->transientLeader;", add)]
assert "pWS == NULL || pWS->pSD == NULL" in add_body
assert "pCD->transientLeader == NULL" in add_body
assert "pcdTop == NULL" in text[add:text.index("void DeleteTransient", add)]
delete = text.index("void DeleteTransient (ClientData *pCD)")
delete_body = text[delete:text.index("    /*", delete)]
assert "pCD == NULL || pCD->transientLeader == NULL" in delete_body
print("transient list helpers validate tree state")
