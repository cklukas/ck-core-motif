#!/usr/bin/env python3
"""Check that Editres string output respects its 16-bit wire length."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "EditresCom.c").read_text(encoding="utf-8")
start = source.index("_XEditResPutString8(stream, str)")
end = source.index("/*\tFunction Name: _XEditResPut8", start)
region = source[start:end]

assert "size_t i, len = strlen(str);" in region
assert "if (len > UINT16_MAX)" in region
assert "len = UINT16_MAX;" in region
assert "_XEditResPut16(stream, len);" in region
assert "XtMalloc(sizeof(unsigned long) *" in source
print("PASS Editres string output preserves wire length")
