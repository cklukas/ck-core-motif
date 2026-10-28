#!/usr/bin/env python3
"""Check modify-verify bridges reject null callback payloads."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c").read_text()
start = source.index("register XmSourceData data = initiator->text.source->data;")
end = source.index("static Boolean ", start + 10)
body = source[start:end]

assert "block->length < 0 || block->ptr == NULL" in body
assert "if (tvcb.text == NULL)" in body
assert "tvcb.text->ptr == NULL" in body
assert "newblock->length > 0 && newblock->ptr == NULL" in body
assert "if (wcs_tvcb.text == NULL)" in body

print("PASS Text modify-verify callbacks guard null payloads")
