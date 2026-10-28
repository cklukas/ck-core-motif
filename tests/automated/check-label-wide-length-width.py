#!/usr/bin/env python3
"""Check that Label wide-string conversion scans with size_t."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Label.c").read_text()
start = source.index("static void\nLabelSetValue")
end = source.index("static int\nLabelPreferredValue", start)
region = source[start:end]

assert "size_t length;" in region
assert "int length;" not in region
assert "while (str2[length] != 0)" in region
assert "(size_t) length > (SIZE_MAX - 1) / (size_t) MB_CUR_MAX" in region
print("Label wide-string conversion uses wide length arithmetic")
