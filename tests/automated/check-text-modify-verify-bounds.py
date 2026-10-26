#!/usr/bin/env python3
"""Ensure Text modify-verify callback copies bound signed lengths."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c"
text = source.read_text(encoding="utf-8")
start = text.index("/* If there are char* callbacks registered")
end = text.index("\n  }  /* end if there are char*", start)
block = text[start:end]

assert "block->length < 0" in block
assert "(size_t)block->length >= (size_t)UINT_MAX" in block
assert "XtMalloc((Cardinal)((size_t)block->length + 1))" in block
assert "newblock->length < 0" in block
assert "XtMalloc((Cardinal)((size_t)newblock->length + 1))" in block
wide_start = text.index("if (initiator->text.wcs_modify_verify_callback)")
wide_end = text.index("\n  /*", wide_start)
wide_block = text[wide_start:wide_end]
assert "UINT_MAX / sizeof(wchar_t)" in wide_block

print("text modify-verify callback buffer bounds: ok")
