#!/usr/bin/env python3
"""Check that standard conversion starts with deterministic empty output."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("XmeStandardConvert(")
end = source.index("Atom\nXmeGetEncodingAtom", start)
region = source[start:end]
assert "cs -> value = NULL;" in region
assert "cs -> type = None;" in region
assert "cs -> format = 8;" in region
assert "cs -> length = 0;" in region
print("standard conversion initializes unsupported-target output")
