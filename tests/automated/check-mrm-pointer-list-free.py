#!/usr/bin/env python3
"""Check Mrm pointer-list cleanup is null-safe and repeatable."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmptrlist.c"
text = source.read_text(encoding="utf-8")
if "if (list_id == NULL)" not in text:
    raise SystemExit("UrmPlistFree lacks null safety")
if "if (list_id == NULL || list_id->ptr_vec == NULL" not in text:
    raise SystemExit("UrmPlistFreeContents lacks null safety")
if "list_id->num_ptrs = 0;" not in text:
    raise SystemExit("UrmPlistFreeContents does not clear ownership state")
print("Mrm pointer-list cleanup is null-safe and repeatable")
