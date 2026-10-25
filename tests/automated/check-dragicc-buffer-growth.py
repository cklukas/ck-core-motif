#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/DragICC.c").read_text()
assert "#include <stdint.h>" in source
assert "SIZE_MAX - buf->size" in source
assert "size_t required = buf->size + (size_t)size;" in source
assert "BYTE *new_bytes;" in source
assert "if (new_bytes == NULL)" in source
assert "buf->bytes = new_bytes;" in source
assert "buf->max = new_max;" in source
print("DragICC write-buffer growth guards present")
