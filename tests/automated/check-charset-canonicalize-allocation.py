#!/usr/bin/env python3
"""Check that legacy charset canonicalization handles allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Obso1_2.c").read_text(encoding="utf-8")
start = source.index("_XmCharsetCanonicalize(")
end = source.index("/*ARGSUSED*/", start)
region = source[start:end]

assert region.count("new_s = XtMalloc") == 3
assert region.count("if (new_s == NULL) return (NULL);") == 3
print("PASS legacy charset canonicalization allocations are guarded")
