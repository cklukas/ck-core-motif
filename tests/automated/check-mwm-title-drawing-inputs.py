#!/usr/bin/env python3
"""Check null-client handling in MWM title drawing."""

from pathlib import Path


source = Path("clients/mwm/WmCDecor.c").read_text(encoding="utf-8")
start = source.index("void DrawWindowTitle (ClientData *pcd, Boolean eraseFirst)")
end = source.index("} /* END OF FUNCTION DrawWindowTitle */", start)
body = source[start:end]

assert "if (pcd == NULL)" in body
assert "return;" in body
assert body.index("if (pcd == NULL)") < body.index("decoration = pcd->decor")

print("PASS MWM title drawing rejects null clients")
