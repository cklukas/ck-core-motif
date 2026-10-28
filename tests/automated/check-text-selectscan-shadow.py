#!/usr/bin/env python3
"""Check Text SelectScan keeps one non-shadowed character pointer."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c").read_text()
prototype = source.index("Scan(")
start = source.index("Scan(", prototype + 1)
end = source.index("/* Source Scan", start)
body = source[start:end]

assert body.count("char * c;") == 1
assert "if (tw->text.char_size == 1) {\n      for" in body

print("Text SelectScan pointer is not shadowed")
