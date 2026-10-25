#!/usr/bin/env python3
"""Check iconv output buffers handle initial and growth allocation failures."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ResEncod.c").read_text()
start = source.index("\nConvertWithIconv(const char")
body = source[start:source.index("#endif", start)]
assert "outp = dest = XtMalloc(outbuf_size);" in body
assert "if (dest == NULL)" in body
assert "new_dest = XtRealloc(dest, outbuf_size);" in body
assert "if (new_dest == NULL)" in body
assert "XtFree(dest);" in body
assert "dest = new_dest;" in body
print("PASS iconv output allocation failures are handled")
