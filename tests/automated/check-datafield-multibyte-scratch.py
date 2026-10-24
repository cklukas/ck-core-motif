#!/usr/bin/env python3
"""Check that DataField wide-character conversion buffers cover MB_LEN_MAX."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
assert "#include <limits.h>" in source
assert source.count("char scratch[MB_LEN_MAX];") == 3
assert "char scratch[8];" not in source
print("DataField multibyte scratch buffers cover MB_LEN_MAX")
