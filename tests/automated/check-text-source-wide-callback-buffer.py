#!/usr/bin/env python3
"""Ensure wide modify-verify callback buffers are initialized and terminated."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/TextStrSo.c").read_text()
start = source.index("size_t converted = mbstowcs(wcs_newblock.wcsptr")
end = source.index("\n    wcs_tvcb.reason", start)
body = source[start:end]
assert "wcs_newblock.wcsptr[0] = (wchar_t)0L;" in body
assert "wcs_newblock.wcsptr[converted] = (wchar_t)0L;" in body
assert body.index("wcsptr[0]") < body.index("wcsptr[converted]")
print("Wide text-source callback buffers are initialized and terminated")
