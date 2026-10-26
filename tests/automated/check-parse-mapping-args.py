#!/usr/bin/env python3
"""Check that XmParseMappingGetValues validates application ArgLists."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "XmStringFunc.c"
text = source.read_text(encoding="utf-8")
for needle in (
    "mapping == NULL || (arg_count != 0 && arg_list == NULL)",
    "if (arg_name == NULL || arg_list[i].value == 0)",
):
    if needle not in text:
        raise SystemExit(f"parse mapping ArgList guard is missing: {needle}")

print("XmParseMappingGetValues ArgList guards present")
