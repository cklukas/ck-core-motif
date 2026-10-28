#!/usr/bin/env python3
"""Check XIM preedit conversion rejects invalid multibyte input."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmIm.c").read_text()
start = source.index("ImPreeditDrawCallback(XIC xic, XPointer client_data")
end = source.index("pb->text = replacement", start)
body = source[start:end]

assert "size_t converted;" in body
assert "converted = mbstowcs" in body
assert "converted == (size_t)-1" in body
assert "converted != (size_t)inserted" in body
assert "if (mbstowcs(replacement" not in body

print("PASS XIM preedit conversion guards")
