#!/usr/bin/env python3
"""Check that the AT-SPI cache snapshot reports allocation failure."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "AccessibleDBus.c"
text = source.read_text(encoding="utf-8")
start = text.index("if (!strcmp(path, CACHEPATH))")
end = text.index("\n  if (!strcmp(path, ROOT))", start)
region = text[start:end]
assert "(XmAccessibleId *)XtMalloc" in region
assert "ids = count ?" in region
assert "count > SIZE_MAX / sizeof(*ids)" in region
assert "if (count != 0 && !ids)" in region
assert "if (count != 0 && !ids)" in region
assert "org.freedesktop.DBus.Error.NoMemory" in region
assert "Unable to snapshot the accessibility tree" in region
print("accessibility cache snapshots report allocation failure")
