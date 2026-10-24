#!/usr/bin/env python3
"""Check Mrm low-level file I/O validates descriptors, buffers, and blocks."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIfile.c"
text = source.read_text(encoding="utf-8")
checks = {
    "Idb__FU_CloseFile": ("if (file_id == NULL)", "close (file_id->file_desc)"),
    "Idb__FU_GetBlock": (
        "if (file_id == NULL || buffer == NULL || block_num <= 0)",
        "file_id->file_desc",
    ),
    "Idb__FU_PutBlock": (
        "if (file_id == NULL || buffer == NULL || block_num <= 0)",
        "file_id->file_desc",
    ),
}
for name, (guard_text, deref_text) in checks.items():
    start = text.rindex("\n" + name + " (") + 1
    end = text.index("\n}\n", start)
    body = text[start:end]
    body = body[body.index("{") + 1:]
    guard = body.index(guard_text)
    deref = body.index(deref_text)
    if guard > deref:
        raise SystemExit(f"{name} validates after I/O dereference")
    if "return MrmFAILURE;" not in body[guard:deref]:
        raise SystemExit(f"{name} failure return is missing")

print("Mrm low-level file I/O validates inputs")
