#!/usr/bin/env python3
"""Check that XmeGetNullCursor validates widget/screen inputs."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Screen.c"
text = source.read_text(encoding="utf-8")
for needle in ("if (w == NULL)\n        return None;", "if (xmScreen == NULL) {"):
    if needle not in text:
        raise SystemExit(f"null-cursor input guard is missing: {needle}")

print("null-cursor input guards present")
