#!/usr/bin/env python3
"""Check null-input handling in MWM system-menu placement."""

from pathlib import Path


source = Path("clients/mwm/WmCDecor.c").read_text(encoding="utf-8")
start = source.index(
    "void GetSystemMenuPosition (ClientData *pcd, int *px, int *py,"
)
end = source.index("} /* END OF FUNCTION GetSystemMenuPosition", start)
body = source[start:end]

assert "if (pcd == NULL || px == NULL || py == NULL)" in body
assert "return;" in body
assert body.index("if (pcd == NULL") < body.index("pcd->clientState")

print("PASS MWM system-menu placement rejects null inputs")
