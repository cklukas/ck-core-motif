#!/usr/bin/env python3
"""Check DataField replacement rejects invalid wide conversion results."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
start = source.index("XmDataFieldReplace(\n        Widget")
body = source[start:start + 5000]

assert "value_length > (size_t)INT_MAX" in body
assert "converted == (size_t)-1" in body
assert "converted > (size_t)INT_MAX" in body
assert "rep_result = False" in body

print("PASS DataField replacement rejects invalid wide conversion")
