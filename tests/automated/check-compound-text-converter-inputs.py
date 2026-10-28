#!/usr/bin/env python3
"""Check compound-text conversion validates Xt converter records."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/ResEncod.c").read_text()
start = source.index("XmCvtTextToXmString(")
end = source.index("    ok = cvtTextToXmString", start)
body = source[start:end]

assert "from_val == NULL" in body
assert "to_val == NULL" in body
assert "from_val->addr == NULL" in body
assert "return( FALSE);" in body

print("PASS compound-text converter inputs are validated")
