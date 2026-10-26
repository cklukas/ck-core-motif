#!/usr/bin/env python3
"""Check null-input handling in MWM frame/client conversion helpers."""

from pathlib import Path


source = Path("clients/mwm/WmCDInfo.c").read_text(encoding="utf-8")


def function_body(marker: str, end_marker: str) -> str:
    start = source.index(marker)
    return source[start : source.index(end_marker, start)]


frame = function_body(
    "void FrameToClient (ClientData *pcd, int *pX, int *pY, unsigned int *pWidth, unsigned int *pHeight)",
    "Boolean GetDepressInfo (ClientData *pcd",
)
client = function_body(
    "void ClientToFrame (ClientData *pcd, int *pX, int *pY, unsigned int *pWidth, unsigned int *pHeight)",
    "Boolean GetDepressInfo (pcd, part, pX, pY, pWidth, pHeight)",
)

guard = (
    "if (pcd == NULL || pX == NULL || pY == NULL ||\n"
    "\tpWidth == NULL || pHeight == NULL)"
)
for body in (frame, client):
    assert guard in body
    assert "return;" in body
    assert body.index(guard) < body.index("pcd->clientOffset")

print("PASS MWM frame/client conversions reject null inputs")
