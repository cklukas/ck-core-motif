#!/usr/bin/env python3
"""Check the text source scratch-buffer multiplication guard."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c"
text = source.read_text()
assert "(SIZE_MAX - 1) / (size_t)tw->text.char_size" in text
assert "block_bytes = (size_t)block->length * (size_t)tw->text.char_size + 1" in text
assert "XtMalloc(block_bytes)" in text
assert "XtRealloc(data->value, block_bytes)" in text
print("text source allocation contract passed")
