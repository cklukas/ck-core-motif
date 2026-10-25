#!/usr/bin/env python3
"""Check transient traversal helpers handle detached clients."""
from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWinList.c"
text = source.read_text()
leader = text.index("ClientData * FindTransientTreeLeader")
assert "if (pcd == NULL)" in text[leader:text.index("} /* END OF FUNCTION FindTransientTreeLeader", leader)]
count = text.index("CountTransientChildren (ClientData *pcd)")
assert "if (pcd == NULL)" in text[count:text.index("} /* END OF FUNCTION CountTransientChildren", count)]
top = text.index("ClientData * FindTransientOnTop")
body = text[top:text.index("#ifdef WSM", top)]
assert "if (pcd == NULL)" in body
assert "return (NULL);" in body
print("transient traversal helpers handle null clients")
