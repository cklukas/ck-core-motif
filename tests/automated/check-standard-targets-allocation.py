#!/usr/bin/env python3
"""Check standard transfer target allocation and failure cleanup."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("XmeStandardTargets(")
end = source.index("\n/*\n * XmeStandardConvert", start)
region = source[start:end]

assert "size_t target_count;" in region
assert "target_count = (size_t)count + builtin_count;" in region
assert "targets = (Atom *)XtMalloc(sizeof(*targets) * target_count);" in region
assert "if (targets == NULL)" in region
assert "*tcount = 0;" in region
assert "XtRealloc((char*) targets" not in region

print("PASS standard transfer target allocation checks failure handling")
