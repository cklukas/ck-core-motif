#!/usr/bin/env python3
"""Check that MWM property processors reject null client records."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmProperty.c").read_text(
    encoding="utf-8"
)
for signature in (
    "void ProcessWmProtocols (ClientData *pCD)",
    "void ProcessMwmMessages (ClientData *pCD)",
    "void ProcessWmColormapWindows (ClientData *pCD)",
):
    start = source.index(signature)
    end = source.index("\n}", start) + 2
    region = source[start:end]
    assert "if (pCD == NULL)" in region
    assert region.index("pCD == NULL") < region.index("XGetWindowProperty")
print("PASS MWM property processors reject null client records")
