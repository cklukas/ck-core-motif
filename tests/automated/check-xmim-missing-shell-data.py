#!/usr/bin/env python3
"""Check that XmIm updates tolerate missing per-shell state."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmIm.c").read_text()
start = source.rindex("set_values(Widget w,")
end = source.index("\nstatic ", start)
body = source[start:end]

assert "im_info = get_im_info(p, False);" in body
assert "if (im_info == NULL)" in body
assert "assert(im_info != NULL)" not in body

print("XmIm missing-shell-data guard: ok")
