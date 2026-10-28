#!/usr/bin/env python3
"""Check Text gap spans are validated before signed pointer conversion."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c").read_text()

assert "static Boolean TextSourceGapBytes" in source
assert "data->gap_end < data->gap_start" in source
assert "*bytes = (size_t)(data->gap_end - data->gap_start);" in source
assert "if (!TextSourceGapBytes(data, &gap_size))" in source

print("Text gap span conversion is validated")
