#!/usr/bin/env python3
"""Check that Screen unpost-behavior lookup validates its widget/screen."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Screen.c"
text = source.read_text(encoding="utf-8")
for needle in ("if (wid == NULL)", "if (xmScreen == NULL)", "XmUNPOST_AND_REPLAY"):
    if needle not in text:
        raise SystemExit(f"unpost-behavior input guard is missing: {needle}")

print("unpost-behavior input guards present")
