#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/Text.c").read_text()

assert "block_length = strlen(block.ptr);" in source
assert "block_length > (size_t)INT_MAX" in source
assert "mb_length = strlen(block.ptr);" in source
assert "mb_length = strlen(mb);" in source
assert "block.length = (int)mb_length;" in source
