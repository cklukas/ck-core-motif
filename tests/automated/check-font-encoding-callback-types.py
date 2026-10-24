#!/usr/bin/env python3
"""Check that FontS encoding callback indexes use pointer-sized conversions."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FontS.c").read_text()
assert "#include <stdint.h>" in source
assert "(XtPointer)(uintptr_t)i" in source
assert "(int)(uintptr_t)data" in source
assert "(XtPointer) current" not in source
assert "(int) data" not in source
print("FontS encoding callback indexes use portable pointer conversions")
