#!/usr/bin/env python3
"""Check copied labels are released by Avatar, Divider, and IconLabel."""
from pathlib import Path

root = Path(__file__).parents[2] / "lib" / "Xm"
expectations = {
    "Avatar.c": "avatar->avatar.initials",
    "Divider.c": "divider->divider.label",
    "IconLabel.c": "label->icon_label.label",
}

for filename, field in expectations.items():
    source = (root / filename).read_text(encoding="utf-8")
    start = source.index("static void Destroy(Widget widget)")
    end = source.index("\n}", start) + 2
    region = source[start:end]
    assert field in region, filename
    assert f"XmStringFree({field});" in region, filename

print("PASS Avatar, Divider, and IconLabel release copied labels")
