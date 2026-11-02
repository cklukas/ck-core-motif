#!/usr/bin/env python3
"""Check Mrm conversion paths handle allocation failures."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
checks = {
    "if (cbptr == NULL)": "translated callback",
    "if (fontlist == NULL)": "font list",
    "if (cbdesc == NULL || bufptr == NULL": "callback descriptor",
    "if (cbptr == NULL)\n    return NULL;": "old callback allocation",
}
for needle, label in checks.items():
    if needle not in text:
        raise SystemExit(f"Mrm {label} allocation/input guard is missing")
assert "if (fontlist == NULL)\n\t\t    return MrmFAILURE;" in text

wide_start = text.index("case MrmRtypeWideCharacter:")
wide_end = text.index("case MrmRtypeFont:", wide_start)
wide = text[wide_start:wide_end]
assert "Cardinal                      wcstr_bytes;" in text
assert "wcstr_bytes = (Cardinal)(sizeof(wchar_t) * (max_size + 1));" in wide
assert "XtMalloc(wcstr_bytes)" in wide
assert "wcstr_bytes = (Cardinal)(sizeof(wchar_t) * (str_size + 1));" in wide
assert "XtRealloc((char *)wcstr_r,\n\t\t\t\t\t    wcstr_bytes)" in wide
invalid_multibyte = wide.index("if (str_size == (size_t)-1)")
assert "XtFree ((char *)wcstr_r);" in wide[invalid_multibyte:]

print("Mrm conversion allocation failures are handled")
