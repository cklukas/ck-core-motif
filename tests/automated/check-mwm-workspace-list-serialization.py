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

build_start = source.index("BuildIconResource(char *buffer, size_t buffer_size")
build_end = source.index("\n}\n\n/* This needs to be called", build_start)
build = source[build_start:build_end]
assert "workspace == NULL" in build
assert "return False;" in build

assert source.count("!= (WmWorkspaceData *)NULL && pWS->name != NULL)") >= 2
print("PASS MWM workspace-list serialization is bounded")
