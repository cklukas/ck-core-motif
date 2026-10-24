#!/usr/bin/env python3
"""Check Mrm pointer-list resize preserves the vector on allocation failure."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmptrlist.c"
text = source.read_text(encoding="utf-8")
start = text.index("UrmPlistResize (")
end = text.find("\n", start + 1)
region = text[start:] if end < 0 else text[start:end]
if "XtPointer\t\t*new_vec" not in region:
    raise SystemExit("Mrm resize lacks a temporary vector")
if region.index("new_vec =") > region.index("if ( new_vec == NULL )"):
    raise SystemExit("Mrm resize checks the old vector instead of temporary result")
if "list_id->ptr_vec = new_vec" not in region:
    raise SystemExit("Mrm resize does not publish a successful temporary vector")
print("Mrm pointer-list resize preserves the original vector on failure")
