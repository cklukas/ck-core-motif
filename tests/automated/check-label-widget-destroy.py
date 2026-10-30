#!/usr/bin/env python3
"""Check copied XmString labels are released by custom widget destructors."""
from pathlib import Path

root = Path(__file__).parents[2] / "lib" / "Xm"
expectations = {
    "Badge.c": ("badge->badge.label", "XmStringFree(badge->badge.label);"),
    "Link.c": ("link->link.label", "XmStringFree(link->link.label);"),
    "Prompt.c": ("prompt->prompt.text", "XmStringFree(prompt->prompt.text);"),
}

for filename, (field, free_call) in expectations.items():
    source = (root / filename).read_text(encoding="utf-8")
    start = source.index("static void Destroy(Widget widget)")
    end = source.index("\n}", start) + 2
    region = source[start:end]
    assert field in region, filename
    assert free_call in region, filename

print("PASS Badge, Link, and Prompt release copied XmString labels")
