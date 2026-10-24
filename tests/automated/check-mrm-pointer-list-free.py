#!/usr/bin/env python3
"""Check Mrm pointer-list cleanup is null-safe and repeatable."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmptrlist.c"
text = source.read_text(encoding="utf-8")
for name in ("UrmPlistFree", "UrmPlistFreeContents"):
    start = text.index(name + " (")
    end = text.find("\n", start + 1)
    region = text[start:] if end < 0 else text[start:end]
    if "if (list_id == NULL)" not in region:
        raise SystemExit(f"{name} lacks null safety")
if "list_id->num_ptrs = 0;" not in text:
    raise SystemExit("UrmPlistFreeContents does not clear ownership state")
print("Mrm pointer-list cleanup is null-safe and repeatable")
