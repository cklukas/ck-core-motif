#!/usr/bin/env python3
"""Check Mrm index entry APIs validate pointers before touching state."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIindexw.c"
text = source.read_text(encoding="utf-8")
checks = {
    "Idb__INX_EnterItem": ("if (file_id == NULL || index == NULL)", "file_id->index_root"),
    "Idb__INX_InitRootLeafRecord": (
        "if (file_id == NULL || buffer_return == NULL)",
        "Idb__BM_InitRecord",
    ),
    "Idb__INX_InitRootNodeRecord": (
        "if (file_id == NULL || buffer_return == NULL || index == NULL)",
        "Idb__BM_InitRecord",
    ),
}
for name, (guard_text, deref_text) in checks.items():
    start = text.rindex(name + " (")
    end = text.index("\n}\n", start)
    region = text[start:end]
    body = region[region.index("{") + 1:]
    guard = body.index(guard_text)
    deref = body.index(deref_text)
    if guard > deref:
        raise SystemExit(f"{name} validates inputs after dereference")
    if "return MrmFAILURE;" not in body[guard:deref]:
        raise SystemExit(f"{name} null-input failure is missing")

print("Mrm index entry APIs validate inputs")
