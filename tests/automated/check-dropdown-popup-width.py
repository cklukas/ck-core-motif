#!/usr/bin/env python3
"""Check DropDown popup width arithmetic cannot wrap on a large offset."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "DropDown.c"
text = source.read_text(encoding="utf-8")
start = text.index("static Boolean\nPopupList(Widget w)")
end = text.index("\n    /*\n     * Now that we know where on the screen", start)
region = text[start:end]
assert "int available_width;" in region
assert "(int)cbw->core.width - (int)temp" in region
assert "if (available_width < 1)" in region
assert "available_width = 1;" in region
assert "width = (Dimension)available_width;" in region
print("drop-down popup width arithmetic is signed and clamped")
