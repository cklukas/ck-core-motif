#!/usr/bin/env python3
"""Check null-input handling in MWM title-text rectangle calculation."""

from pathlib import Path


source = Path("clients/mwm/WmCDecor.c").read_text(encoding="utf-8")
start = source.index("void GetTextBox (ClientData *pcd, XRectangle *pBox)")
end = source.index("void DrawWindowTitle", start)
body = source[start:end]

assert "if (pcd == NULL || pBox == NULL)" in body
assert "return;" in body
assert body.index("if (pcd == NULL") < body.index("GetFramePartInfo")

print("PASS MWM title-text rectangle rejects null inputs")
