#!/usr/bin/env python3
"""Check DataField wide-length validation precedes sentinel reads."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
start = source.index("df_FindPixelLength(\n        XmDataFieldWidget")
end = source.index("\nstatic ", start)
body = source[start:end]

assert body.count("wc_tmp = wc_string[length];") == 2
assert body.count("if (length < 0 || (size_t)length >") >= 2
for sentinel in ["wc_tmp = wc_string[length];"]:
    assert body.index(sentinel) > body.index("if (length < 0 || (size_t)length >")

print("DataField validates wide-string length before sentinel reads")
