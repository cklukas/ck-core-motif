#!/usr/bin/env python3
"""Check that ValidateString never copies through an unchecked XtMalloc."""

from pathlib import Path


source = Path("lib/Xm/TextF.c").read_text(encoding="utf-8")

assert "memcpy(XtMalloc((unsigned)(tf->text.string_length + 30))" not in source
assert "memcpy(XtMalloc((unsigned)\n\t\t\t\t\t\t\t    tf->text.size_allocd)" not in source
assert "char *validated_value;" in source
assert "wchar_t *validated_wc_value = (wchar_t *)XtMalloc(" in source
assert "if (validated_value == NULL)" in source
assert "if (validated_wc_value == NULL)" in source

print("TextField validation allocation checks passed")
