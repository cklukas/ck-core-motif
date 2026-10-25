#!/usr/bin/env python3
"""Check MWM session database-name copies reject size overflow safely."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmXSMP.c").read_text(
    encoding="utf-8"
)
helper = source[source.index("copyClientDBName") : source.index("/*\n", source.index("copyClientDBName"))]
assert "length > SIZE_MAX - 1 - EXTRA_FN_CHARS" in helper
assert "length > (size_t)UINT_MAX - 1 - EXTRA_FN_CHARS" in helper
assert "XtMalloc((Cardinal)(length + 1 + EXTRA_FN_CHARS))" in helper
assert "XtMalloc(length + 1 + EXTRA_FN_CHARS)" not in helper
assert "memcpy(copy, source, length + 1)" in helper
body = source[source.index("static void\ngetClientDBName") : source.index("/*\n *  See comments above", source.index("static void\ngetClientDBName"))]
assert body.count("copyClientDBName(") == 2
assert "strlen(*argP) + 1 +" not in body
assert "strlen(wmGD.sessionClientDB) + 1 +" not in body

print("MWM database-name allocation is overflow checked")
