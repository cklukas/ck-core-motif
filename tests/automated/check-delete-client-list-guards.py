#!/usr/bin/env python3
"""Check that DeleteClientFromList validates detached list state."""
from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWinList.c"
text = source.read_text()
start = text.index("void DeleteClientFromList (WmWorkspaceData *pWS, ClientData *pCD)")
end = text.index("} /* END OF FUNCTION DeleteClientFromList */", start)
body = text[start:end]
assert "pWS == NULL || pCD == NULL || pWS->pSD == NULL" in body
assert "pWsc = GetWsClientData (pWS, pCD);" in body
assert "pWsc != NULL && !pWsc->pIconBox" in body
print("DeleteClientFromList validates workspace and client state")
