#!/usr/bin/env python3
"""Check deterministic Mrm record-buffer outputs on failure."""

import os
from pathlib import Path

source = Path(os.environ.get("MOTIF_IBUFFER_SOURCE", Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIbuffer.c"))
text = source.read_text(encoding="utf-8")
functions = ("Idb__BM_GetBuffer", "Idb__BM_GetRecord", "Idb__BM_InitRecord", "Idb__BM_InitDataRecord")
for name in functions:
    start = text.index(name + " (")
    end = text.index("\n}", start)
    body = text[start:end]
    guard = body.index("if (file_id == NULL || buffer_return == NULL")
    clear = body.index("*buffer_return = NULL;", guard)
    if clear < guard:
        raise SystemExit(f"{name} clears output before its input guard")
    if "return MrmFAILURE;" not in body[guard:clear + 40]:
        raise SystemExit(f"{name} does not retain its input guard")
get_buffer = text.index("Idb__BM_GetBuffer (")
get_body = text[get_buffer:text.index("\n}", get_buffer)]
pool_guard = get_body.index("if (idb__buffer_pool_size <= 0)")
pool_use = get_body.index("idb__buffer_pool_vec", pool_guard)
if pool_guard > pool_use:
    raise SystemExit("GetBuffer checks pool size after using the pool")
print("Mrm record-buffer outputs are initialized before failure paths")
