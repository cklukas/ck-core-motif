#!/usr/bin/env python3
"""Check ToggleButton redisplay geometry uses signed dimension arithmetic."""
from pathlib import Path

root = Path(__file__).resolve().parents[2] / "lib" / "Xm"
checks = {
    "ToggleB.c": ("(int)XtWidth(tb) - x - offset", "(int)XtHeight(tb) - y - offset"),
    "ToggleBG.c": ("(int)tb->rectangle.width - x - offset", "(int)tb->rectangle.height - y - offset"),
}
for filename, fragments in checks.items():
    text = (root / filename).read_text(encoding="utf-8")
    start = text.index("redisplayPixmap(")
    end = text.index("\f", start)
    body = text[start:end]
    for fragment in fragments:
        if fragment not in body:
            raise SystemExit(f"{filename} redisplay bound is not signed: {fragment}")
print("ToggleButton redisplay bounds use signed arithmetic")
