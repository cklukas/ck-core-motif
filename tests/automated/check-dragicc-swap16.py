#!/usr/bin/env python3
"""Check Drag ICC 16-bit swapping preserves field representation."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragICCI.h").read_text()
start = source.index("#define swap2bytes")
end = source.index("\n\n\ntypedef struct", start)
body = source[start:end]

assert "unsigned char swap_bytes[2];" in body
assert "memcpy(swap_bytes, &(s), sizeof(swap_bytes));" in body
assert "memcpy(&(s), swap_bytes, sizeof(swap_bytes));" in body
assert "bit16.t" not in body

print("Drag ICC 16-bit swapping preserves field representation")
