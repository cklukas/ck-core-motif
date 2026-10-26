#!/usr/bin/env python3
"""Ensure Text RTL translation setup handles bounded allocation failure."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Text.c"
text = source.read_text(encoding="utf-8")
start = text.index("\n  /* Translation table overwrite */")
end = text.index("\n  /* Initialize table */", start)
block = text[start:end]

assert "size_t vevent_bindings_len" in block
assert "vevent_bindings_len <= SIZE_MAX - 1" in block
assert "vevent_bindings_len + 1 <= UINT_MAX" in block
assert "XtMalloc((Cardinal)(vevent_bindings_len + 1))" in block
assert "!= NULL" in block
assert "strcpy(vevent_bindings" in block

print("text RTL translation allocation guard: ok")
