#!/usr/bin/env python3
from pathlib import Path
source = Path(__file__).parents[2] / "clients/mwm/WmProperty.c"
text = source.read_text()
start = text.index("SetWorkspaceListProperty (WmScreenData *pSD)")
body = text[start:text.index("} /* END OF FUNCTION SetWorkspaceListProperty", start)]
assert "pSD == NULL" in body
assert "pSD->numWorkspaces > 0 && pSD->pWS == NULL" in body
assert "if (pSD->numWorkspaces > 0)" in body
start = text.index("SetCurrentWorkspaceProperty (WmScreenData *pSD)")
body = text[start:text.index("} /* END OF FUNCTION SetCurrentWorkspaceProperty", start)]
assert "pSD == NULL || pSD->pActiveWS == NULL" in body
print("workspace property setters validate detached state")
