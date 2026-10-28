#!/usr/bin/env python3
"""Check XPM token scans reject and consume overlong tokens safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmdata.c").read_text()
start = source.index("xpmNextWord(")
region = source[start : source.index("/*\n * skip whitespace and compute", start)]

assert "if (buf == NULL || buflen == 0)" in region
assert "if (n == buflen && c != '\\0'" in region
assert "while ((c = (unsigned char)*data->cptr) != '\\0'" in region
assert "if (n == buflen)" in region
assert "while ((c = Getc(data, file)) != data->Eos" in region
print("XPM token scans reject and consume overlong tokens")
