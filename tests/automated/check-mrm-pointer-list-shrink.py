#!/usr/bin/env python3
"""Check Mrm pointer-list resize rejects truncating active entries."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmptrlist.c"
text = source.read_text(encoding="utf-8")
start = text.index("UrmPlistResize (")
end = text.find("\n", start + 1)
region = text[start:] if end < 0 else text[start:end]
needle = "if (size < list_id->num_ptrs)"
if needle not in region:
    raise SystemExit("Mrm pointer-list resize allows truncating active entries")
if region.index(needle) > region.index("new_vec ="):
    raise SystemExit("Mrm pointer-list resize checks shrink after reallocating")
print("Mrm pointer-list resize preserves all active entries")
