#!/usr/bin/env python3
"""Check that MWM client workspace-list allocation is width checked."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWinInfo.c").read_text(encoding="utf-8")
start = source.index("/* \n     * Allocate initial workspace ID list")
end = source.index("    pCD->sizeWsList", start)
region = source[start:end]
assert "pCD->pSD == NULL" in region
assert "numWorkspaces < 0" in region
assert "UINT_MAX / sizeof(WsClientData)" in region
assert "XtMalloc((Cardinal)((size_t)pCD->pSD->numWorkspaces * sizeof(WsClientData)))" in region
assert "XtMalloc(pCD->pSD->numWorkspaces * sizeof(WsClientData))" not in region
print("PASS MWM client workspace-list allocation is width checked")
