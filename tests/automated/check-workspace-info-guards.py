#!/usr/bin/env python3
from pathlib import Path
source = Path(__file__).parents[2] / "clients/mwm/WmProperty.c"
text = source.read_text()
start = text.index("void SetWorkspaceInfo")
body = text[start:text.index("} /* END OF FUNCTION SetWorkspaceInfo", start)]
assert "cInfo > 0 && pWsInfo == NULL" in body
assert "cInfo > (size_t)-1 / sizeof(WorkspaceInfo)" in body
assert "size_t bytes" in body
assert "bytes / sizeof(long)" in body
print("workspace-info serialization validates counted data")
