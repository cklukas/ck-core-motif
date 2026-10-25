#!/usr/bin/env python3
"""Check traversal geometry helpers validate widget and rectangle inputs."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "Traversal.c").read_text(
    encoding="utf-8"
)
checks = (
    ("_XmComputeVisibilityRect(Widget", "w == NULL || rectPtr == NULL"),
    ("_XmSetRect(\n", "rect == NULL || w == NULL"),
    ("_XmIntersectRect(\n", "srcRectA == NULL || widget == NULL || dstRect == NULL"),
    ("_XmEmptyRect(\n", "r == NULL"),
    ("_XmClearRect(\n", "r == NULL"),
)
for name, needle in checks:
    start = text.index(name)
    body = text[start : start + 900]
    if needle not in body:
        raise SystemExit(f"geometry input guard is missing: {name}")

print("traversal geometry input guards present")
