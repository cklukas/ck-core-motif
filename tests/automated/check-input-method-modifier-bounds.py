#!/usr/bin/env python3
"""Check bounded XmNinputMethod locale-modifier construction."""
from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "XmIm.c").read_text(encoding="utf-8")
start = text.index("get_xim_info")
end = text.index("XtGetApplicationNameAndClass", start)
region = text[start:end]
assert "cp_length = strlen(cp)" in region
assert "cp_length <= (size_t)UINT_MAX - 5" in region
assert "memcpy(tmp, \"@im=\", 4)" in region
assert "XSetLocaleModifiers(tmp)" in region
assert "strcat(tmp,cp)" not in region
assert "assert(strlen(tmp) < BUFSIZ)" not in region
print("input-method locale modifier construction is bounded")
