#!/usr/bin/env python3
"""Check that Screen drag-icon helpers validate widget/screen inputs."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Screen.c"
text = source.read_text(encoding="utf-8")
if text.count("if (w == NULL)") < 3 or text.count("if (xmScreen == NULL)") < 4:
    raise SystemExit("Screen icon input guards are incomplete")

print("Screen icon input guards present")
