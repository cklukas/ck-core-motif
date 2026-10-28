#!/usr/bin/env python3
"""Check length-bounded ColorObj property parsing."""
from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorObj.c").read_text(encoding="utf-8")
start = text.index("static Boolean\nFetchPixelData(Widget w, char * value, unsigned long")
end = text.index("static Boolean\nColorCachePropertyExists", start)
region = text[start:end]
assert "unsigned long value_length" in region
assert "bounded_value = (char *)XtMalloc" in region
assert "memcpy(bounded_value, value" in region
assert "ParseColorHexField(&cursor" in region
assert "strtoul(*cursor, &end, 16)" in text
assert "if (value != NULL && length > 0)" in text
assert "value[length - 1]" in text
print("ColorObj property parsing is length-bounded")
