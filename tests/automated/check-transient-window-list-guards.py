#!/usr/bin/env python3
"""Check transient window-list construction validates its inputs."""
from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWinList.c"
text = source.read_text()
start = text.index("Window * MakeTransientWindowList")
body = text[start:text.index("    pcdNext = pcd->transientChildren", start)]
assert "windows == NULL || pcd == NULL" in body
assert "return (windows);" in body
print("transient window-list construction validates buffers and clients")
