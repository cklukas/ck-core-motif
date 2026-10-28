#!/usr/bin/env python3
"""Check XPM extension writing validates inputs and propagates status."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmWrFFrI.c").read_text()
assert "LFUNC(WriteExtensions, int" in source
assert "ErrorStatus = WriteExtensions(file, info->extensions, info->nextensions);" in source
start = source.index("WriteExtensions(\n")
region = source[start : source.index("#ifndef NO_ZPIPE", start)]
assert "file == NULL || (num != 0 && ext == NULL)" in region
assert "ext->name == NULL" in region
assert "ext->lines == NULL" in region
assert "*line == NULL" in region
assert "if (ferror(file))" in region
print("XPM extension writing validates inputs and propagates status")
