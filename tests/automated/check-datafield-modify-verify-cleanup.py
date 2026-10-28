#!/usr/bin/env python3
"""Check invalid DataField wide callback data reaches cleanup."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
start = source.index("if (XmTextF_wcs_modify_verify_callback(tf))")
end = source.index("static void", start)
body = source[start:end]

assert "if (wcs_vcb.text == NULL)" in body
assert "wcs_vcb.text->wcsptr == NULL" in body
assert "goto modify_verify_allocation_failed;" in body
assert "if (wcs_do_free)" in body
print("DataField modify-verify invalid callback data is cleaned up")
