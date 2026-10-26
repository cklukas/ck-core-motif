#!/usr/bin/env python3
"""Check defensive input handling in MWM depressed-gadget geometry."""

from pathlib import Path


source = Path("clients/mwm/WmCDInfo.c").read_text(encoding="utf-8")
start = source.index(
    "Boolean GetDepressInfo (ClientData *pcd, int part, int *pX, int *pY,"
)
end = source.index("/*************************************<->", start + 1)
body = source[start:end]

guard = (
    "if (pcd == NULL || pX == NULL || pY == NULL ||\n"
    "\tpWidth == NULL || pHeight == NULL || pInvertWidth == NULL)"
)
assert guard in body
assert "return (FALSE);" in body
assert body.index(guard) < body.index("pcd->decor")

print("PASS MWM depressed-gadget geometry rejects null inputs")
