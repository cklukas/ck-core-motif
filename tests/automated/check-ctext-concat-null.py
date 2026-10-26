#!/usr/bin/env python3
"""Check that zero-length compound-text appends avoid NULL memcpy."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "ResEncod.c"
text = source.read_text(encoding="utf-8")
start = text.index("static OctetPtr \nctextConcat(")
start = text.index("{", start)
end = text.index("\n}\n", start)
region = text[start:end]

required = (
    "str2 == NULL && str2len != 0",
    "if (str2len != 0)",
    "memcpy(&new_str[str1len], str2, str2len);",
)
missing = [item for item in required if item not in region]
if missing:
    raise SystemExit("compound-text NULL append guard missing: " + ", ".join(missing))

print("compound-text zero-length NULL append is safe")
