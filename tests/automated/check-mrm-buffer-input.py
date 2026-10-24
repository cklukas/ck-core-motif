#!/usr/bin/env python3
"""Check Mrm record-buffer entry points validate file and output pointers."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIbuffer.c"
text = source.read_text(encoding="utf-8")
checks = {
    "Idb__BM_GetBuffer": "if (file_id == NULL || buffer_return == NULL)",
    "Idb__BM_GetRecord": "if (file_id == NULL || buffer_return == NULL)",
    "Idb__BM_InitRecord": "if (file_id == NULL || buffer_return == NULL)",
    "Idb__BM_InitDataRecord": "if (file_id == NULL || buffer_return == NULL)",
}
for name, guard_text in checks.items():
    start = text.rindex("\n" + name + " (") + 1
    end = text.index("\n}\n", start)
    region = text[start:end]
    body = region[region.index("{") + 1:]
    guard = body.index(guard_text)
    if "return MrmFAILURE;" not in body[guard:guard + 140]:
        raise SystemExit(f"{name} null-input failure is missing")
    if name in ("Idb__BM_GetBuffer", "Idb__BM_GetRecord"):
        deref = body.index("idb__buffer_pool_vec")
    elif name == "Idb__BM_InitRecord":
        deref = body.index("Idb__BM_GetBuffer")
    else:
        deref = body.index("Idb__BM_InitRecord")
    if guard > deref:
        raise SystemExit(f"{name} validates after a pointer dereference")

print("Mrm record-buffer entry points validate inputs")
