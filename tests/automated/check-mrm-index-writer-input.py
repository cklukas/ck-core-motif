#!/usr/bin/env python3
"""Check Mrm index insertion validates contexts before strlen(index)."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIindexw.c"
text = source.read_text(encoding="utf-8")
for name in ("Idb__INX_EnterLeafIndex", "Idb__INX_EnterNodeIndex"):
    start = text.index(name + " (", text.index("Cardinal", text.index(name)))
    next_function = text.index("\n}\n", start)
    region = text[start:next_function]
    guard = region.index("if (file_id == NULL || buffer == NULL || index == NULL)")
    strlen = region.index("strlen(index)")
    if guard > strlen:
        raise SystemExit(f"{name} dereferences index before validation")
    if "return MrmFAILURE;" not in region[guard:strlen]:
        raise SystemExit(f"{name} null-input failure is missing")

print("Mrm index writers validate contexts and index strings")
