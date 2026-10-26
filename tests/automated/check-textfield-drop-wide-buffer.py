#!/usr/bin/env python3
"""Check TextField drop conversion bounds its wide-character buffer."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextFSel.c").read_text()
start = source.index("static void \nDropTransferProc")
end = source.index("\n#define CR1228", start)
body = source[start:end]

assert "wc_total_length < 0" in body
assert "UINT_MAX / sizeof(wchar_t) - 1" in body
assert "if (wc_total_value == NULL)" in body
assert "XmTransferDone(ds->transfer_id, XmTRANSFER_DONE_FAIL);" in body

print("TextField drop wide-character buffers are bounded and checked")
