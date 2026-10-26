#!/usr/bin/env python3
"""Check XPM default-color duplication fails transactionally."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmscan.c").read_text()
start = source.index("ScanOtherColors(\n")
end = source.index("\n}\n", start) + 2
body = source[start:end]

assert "defaults[key] = (char *) xpmstrdup(s);" in body
assert "if (!defaults[key])" in body
assert "return (XpmNoMemory);" in body
print("PASS XPM default-color allocation failure is handled")
