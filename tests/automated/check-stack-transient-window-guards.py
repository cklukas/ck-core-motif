#!/usr/bin/env python3
"""Check StackTransientWindow validates transient ancestry."""
from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWinList.c"
text = source.read_text()
start = text.index("void StackTransientWindow (ClientData *pcd)")
end = text.index("} /* END OF FUNCTION StackTransientWindow", start)
body = text[start:end]
assert "pcd == NULL || pcd->transientLeader == NULL" in body
assert "pcdPrev != NULL" in body
assert "if (pcdPrev == NULL)" in body
print("StackTransientWindow validates transient ancestry")
