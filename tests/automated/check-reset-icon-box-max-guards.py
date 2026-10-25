#!/usr/bin/env python3
"""Check icon-box maximum-size reset validates its inputs."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmIconBox.c"
text = source.read_text(encoding="utf-8")

assert "void ResetIconBoxMaxSize (ClientData *pCD, Widget bBoardWidget)" in text
assert "if (pCD == NULL || bBoardWidget == NULL)" in text
print("icon-box maximum-size reset validates client and widget inputs")
