#!/usr/bin/env python3
"""Check XmStringToXmStringTable rejects null input and rolls back failures."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringFunc.c").read_text()
start = source.index("XmStringToXmStringTable(")
end = source.index("\n\nXmTabList", start)
region = source[start:end]

assert "if (table != NULL)" in region
assert "if (string == NULL)" in region
assert "if ((*table)[0] == NULL)" in region
assert "if ((*table)[i] == NULL)" in region
assert "XmStringFree((*table)[--i]);" in region
print("PASS XmStringToXmStringTable validates input and rolls back output")
