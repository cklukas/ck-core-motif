#!/usr/bin/env python3
"""Check XmString concatenation guards line-count allocation arithmetic."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()

assert "if (a_lc > UINT_MAX - b_lc + 1)" in source
assert "if (a_lc > UINT_MAX - b_lc)" in source
assert "(size_t)lc > SIZE_MAX / sizeof(_XmStringEntry)" in source
assert "(Cardinal)(sizeof(_XmStringEntry) * (size_t)lc)" in source

print("XmString concatenation allocation bounds are guarded")
