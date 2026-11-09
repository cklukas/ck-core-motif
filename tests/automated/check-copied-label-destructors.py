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

icon_label = (root / "IconLabel.c").read_text(encoding="utf-8")
start = icon_label.index("static Boolean SetValues(")
end = icon_label.index("\nstatic XtGeometryResult QueryGeometry", start)
set_values = icon_label[start:end]
assert "XmStringFree(old->icon_label.label);" in set_values

print("PASS copied labels are released during destruction and IconLabel replacement")
