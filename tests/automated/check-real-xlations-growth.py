#!/usr/bin/env python3
"""Check growable storage for generated real translation strings."""
from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "Obso2_0.c").read_text(encoding="utf-8")
start = text.index("static Boolean\nXmAppendTranslationText")
end = text.index("/************************************************************************", text.index("_XmGetRealXlations", start))
region = text[start:end]
assert "size_t used" in region
assert "capacity = 256" in region
assert "while (*capacity < required)" in region
assert "SIZE_MAX" in region
assert "*used == SIZE_MAX" in region
assert "required > (size_t)UINT_MAX" in region
assert "XtRealloc(*buffer" in region
assert "char *result, buf[1000]" not in region
assert "strcat(tmp" not in region
assert "assert((tmp - buf) < 1000)" not in region
print("real translation generation uses growable checked storage")
