#!/usr/bin/env python3
"""Check text-source wide initialization validates wcstombs output."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c").read_text()
start = source.index("converted_bytes = value == NULL ? 0 :")
end = source.index("  } else {", start)
body = source[start:end]

assert "converted_bytes == (size_t)-1 || converted_bytes >= allocation_bytes" in body
assert "tmp_value[converted_bytes] = '\\0';" in body
assert "converted_bytes < 0" not in body

print("PASS text-source wide conversion guard")
