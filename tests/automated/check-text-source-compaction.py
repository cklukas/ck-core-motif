#!/usr/bin/env python3
"""Ensure text-source gap compaction keeps valid state on realloc failure."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c"
text = source.read_text(encoding="utf-8")
start = text.index("  if (data->maxlength != TEXT_INITIAL_INCREM")
end = text.index("\n  return EditDone;", start)
block = text[start:end]

assert "compact_ptr = XtRealloc" in block
assert "if (compact_ptr != NULL)" in block
assert "data->maxlength = compact_maxlength" in block
assert "UINT_MAX / (size_t)char_size" in block
assert "data->gap_start = data->ptr" in block

print("text-source compaction allocation guard: ok")
