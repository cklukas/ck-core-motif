#!/usr/bin/env python3
"""Ensure pending-state sentinel handling uses pointer-width integer casts."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c"
text = source.read_text(encoding="utf-8")
start = text.index("void\n_XmStringSourceSetPending")
end = text.index("\n/*", start)
block = text[start:end]

assert "(uintptr_t)pending > (uintptr_t)1" in block
assert "(Boolean)(uintptr_t)pending" in block
assert "(long)pending" not in block

print("text-source pending pointer-width sentinel: ok")
