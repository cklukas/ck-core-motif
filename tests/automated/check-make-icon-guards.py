#!/usr/bin/env python3
"""Check that MakeIcon rejects detached workspace/client state."""
from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmIDecor.c"
text = source.read_text()
start = text.index("Boolean MakeIcon (WmWorkspaceData *pWS, ClientData *pcd)")
body = text[start:text.index("    XSetWindowAttributes", start)]
assert "if (pWS == NULL || pcd == NULL)" in body
assert "return False;" in body
print("MakeIcon validates workspace and client state")
