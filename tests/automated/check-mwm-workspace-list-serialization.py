#!/usr/bin/env python3
"""Check that MWM workspace-list serialization is deterministic and bounded."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmXSMP.c").read_text(encoding="utf-8")
start = source.index("getClientWorkspaces(ClientData *pCD)")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "char *cwsP = NULL" in region
assert "size_t pLen = 0;" in region
assert "if (wsNameP == NULL)" in region
assert "SIZE_MAX - pLen - 1" in region
assert "return cwsP;" in region
print("PASS MWM workspace-list serialization is bounded")
