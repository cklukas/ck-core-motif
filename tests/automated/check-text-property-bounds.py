#!/usr/bin/env python3
"""Check text-property conversion count and terminator bounds."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c"
text = source.read_text()
assert "count < 0" in text
assert "(count != 0 && string_table == NULL)" in text
assert "*ubufptr = '\\0';" in text
assert "*(++ubufptr) = '\\0';" not in text
print("text property conversion bounds contract passed")
