#!/usr/bin/env python3
"""Check TextField transfer lengths use their proven bounded types."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextFSel.c").read_text(
    encoding="utf-8"
)
start = source.index("  } else { /* it must be either TEXT")
end = source.index("\n  }\n  \n  if (!replace_res)", start)
body = source[start:end]
assert "*length > (unsigned long)INT_MAX" in body
assert "num_chars = (int)*length;" in body
assert "(int)*length, True" in body
assert "XtMalloc((Cardinal)((size_t)*length + 1))" in body
assert "(size_t)*length + 1" in body
assert "(unsigned) *length" not in body
print("PASS TextField transfer lengths use explicit bounded conversions")
