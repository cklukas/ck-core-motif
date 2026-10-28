#!/usr/bin/env python3
"""Keep DataField wide selection byte counts failure- and overflow-safe."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/DataF.c").read_text()
start = source.index("_XmDataFieldCountBytes(")
end = source.index("\n/*ARGSUSED*/", start)
body = source[start:end]
assert "int char_bytes = wctomb(tmp, *wc_ptr);" in body
assert "char_bytes < 0 || char_bytes > INT_MAX - n_bytes" in body
assert "n_bytes += char_bytes;" in body
print("DataField selection byte counts reject conversion failure and overflow")
