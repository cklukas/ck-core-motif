#!/usr/bin/env python3
"""Check XmString line-entry growth respects Xt's Cardinal size."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/XmString.c").read_text(
    encoding="utf-8"
)

assert "(size_t)(lc + 1) > UINT_MAX / sizeof(_XmStringEntry)" in source
assert "new_entries == NULL" in source
print("XmString line-entry growth is bounded")
