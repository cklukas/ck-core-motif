#!/usr/bin/env python3
"""Check Mrm record-buffer entry points validate file and output pointers."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIbuffer.c"
text = source.read_text(encoding="utf-8")
checks = {
    "Idb__BM_GetBuffer": "if (file_id == NULL || buffer_return == NULL)",
    "Idb__BM_GetRecord": "if (file_id == NULL || buffer_return == NULL || record <= 0)",
    "Idb__BM_InitRecord": "if (file_id == NULL || buffer_return == NULL)",
    "Idb__BM_InitDataRecord": "if (file_id == NULL || buffer_return == NULL)",
}
for name, guard_text in checks.items():
    start = text.index(name + " (")
    next_start = text.find("\n\f", start)
    region = text[start:] if next_start < 0 else text[start:next_start]
    guard = region.index(guard_text)
    if "return MrmFAILURE;" not in region[guard:guard + 180]:
        raise SystemExit(f"{name} null-input failure is missing")
    if name in ("Idb__BM_GetBuffer", "Idb__BM_GetRecord"):
        deref = region.index("idb__buffer_pool_vec")
    elif name == "Idb__BM_InitRecord":
        deref = region.index("Idb__BM_GetBuffer")
    else:
        deref = region.index("Idb__BM_InitRecord")
    if guard > deref:
        raise SystemExit(f"{name} validates after a pointer dereference")

print("Mrm record-buffer entry points validate inputs")
