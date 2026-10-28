#!/usr/bin/env python3
"""Check SelectionBox callback lengths preserve their signed ABI safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "SelectioB.c").read_text()
start = source.index("static void \nSelectionBoxCallback")
end = source.index("static", start + 24)
region = source[start:end]

assert "unsigned int value_length = XmStringLength(temp.value);" in region
assert "value_length > (unsigned int)INT_MAX ? INT_MAX" in region
assert "(int)value_length" in region
print("SelectionBox callback string length is bounded at the int ABI")
