#!/usr/bin/env python3
"""Check the charset-table converter's allocation-width guard."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c").read_text()
start = source.index("static Boolean\nCvtStringToCharSetTable")
body = source[start:source.index("_XM_CONVERTER_DONE", start)]
assert "tableSize > (size_t)UINT_MAX" in body
assert "strDataSize > (size_t)UINT_MAX - tableSize" in body
assert body.index("tableSize > (size_t)UINT_MAX") < body.index("strDataSize > (size_t)UINT_MAX - tableSize")
print("PASS charset-table allocation width is bounded")
