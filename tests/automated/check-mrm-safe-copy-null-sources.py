#!/usr/bin/env python3
"""Check Mrm callback value copies reject null pointer sources safely."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nUrm__CW_SafeCopyValue (")
end = text.find("\n\n\f", start)
region = text[start:] if end < 0 else text[start:end]
for needle in (
    "if (vec_count > 0 && cstr_table_src == NULL)",
    "if (vec_count > 0 && char8_table_src == NULL)",
    "if (char8_src == NULL)",
    "if (cstr_src == NULL)",
    "if (wchar_src == NULL)",
    "if (single_float_src == NULL)",
    "if (float_src == NULL)",
):
    if needle not in region:
        raise SystemExit(f"missing null-source guard: {needle}")
print("PASS Mrm safe callback copies guard null sources")
