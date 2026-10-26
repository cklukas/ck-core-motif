#!/usr/bin/env python3
"""Check I18List visible-height arithmetic is widened before narrowing."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text()
start = source.index("SetVisibleSize(Widget w, Boolean set_width)")
end = source.index("\n/*\tFunction Name: CalcColumnInfo", start)
region = source[start:end]

assert "uint64_t\t\tcontent_height;" in region
assert "uint64_t\t\ttotal_height;" in region
assert "visible_rows > 0 ? visible_rows : 0" in region
assert "total_height = title_height + content_height + VERTICAL_SPACE;" in region
assert "total_height > USHRT_MAX" in region
assert "(Dimension)total_height" in region
print("PASS I18List visible height uses bounded wide arithmetic")
