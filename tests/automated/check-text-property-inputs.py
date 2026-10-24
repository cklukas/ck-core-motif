#!/usr/bin/env python3
"""Check reverse text-property input and count guards."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c"
text = source.read_text()
assert "string_table_return == NULL" in text
assert "count_return == NULL" in text
assert "text_prop->nitems < 0" in text
assert "text_prop->value == NULL" in text
assert "*count_return == INT_MAX" in text
print("reverse text property input contract passed")
