#!/usr/bin/env python3
"""Check Mrm pointer-list append validates vector and counter state."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmptrlist.c"
text = source.read_text(encoding="utf-8")
start = text.index("UrmPlistAppendPointer (")
end = text.find("\n", start + 1)
region = text[start:] if end < 0 else text[start:end]
for needle in ("list_id->ptr_vec == NULL", "list_id->num_ptrs < 0",
               "list_id->num_ptrs > list_id->num_slots"):
    if needle not in region:
        raise SystemExit(f"Mrm append lacks state validation: {needle}")
print("Mrm pointer-list append validates vector and counter state")
