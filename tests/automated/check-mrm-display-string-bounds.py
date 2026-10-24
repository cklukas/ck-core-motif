#!/usr/bin/env python3
"""Check Mrm display-string conversion validates its copy contract."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
start = text.index("Urm__CW_DisplayToString (")
end = text.index("\n}\n", start)
region = text[start:end]
for needle in (
    "val == NULL || add_string == NULL || add_string_size <= 0",
    "add_string_len + 1 > (size_t)add_string_size",
    "(size_t)dpysize > SIZE_MAX - (size_t)add_string_size",
    "memcpy (&return_val[count], add_string, add_string_len + 1);",
):
    if needle not in region:
        raise SystemExit(f"Mrm display-string bound is missing: {needle}")
if "strcat (&return_val[count], add_string)" in region:
    raise SystemExit("unchecked display-string concatenation remains")

print("Mrm display-string conversion validates its copy contract")
