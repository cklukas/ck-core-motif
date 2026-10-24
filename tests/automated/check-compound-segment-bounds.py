#!/usr/bin/env python3
"""Check that compound-string segment decoration uses checked size arithmetic."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c").read_text()
start = source.index("/* Before returning Success, check to see whether we need")
end = source.index("\n\treturn(return_status);", start)
region = source[start:end]

assert "size_t newlength;" in region
assert "size_t prefix_length" in region
assert "SIZE_MAX - prefix_length - 1U" in region
assert "int newlength;" not in region
print("compound-string segment decoration checks size bounds")
