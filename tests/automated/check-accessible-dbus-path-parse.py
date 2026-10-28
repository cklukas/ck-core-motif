#!/usr/bin/env python3
"""Check AT-SPI object-path identifiers are parsed without narrowing bugs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "AccessibleDBus.c").read_text()
start = source.index('else if (!strncmp(path, PREFIX "/", sizeof(PREFIX)))')
end = source.index("\n  node = _XmAccessibilityNode", start)
region = source[start:end]
assert "isdigit((unsigned char)*id_text)" in region
assert "errno = 0" in region
assert "errno != ERANGE" in region
assert "parsed <= UINT32_MAX" in region
assert "strtoul(path + sizeof(PREFIX)" not in region
print("AT-SPI object paths reject malformed and overflowing identifiers")
