#!/usr/bin/env python3
"""Check Mrm open APIs clear output handles before attempting open."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmItop.c"
text = source.read_text(encoding="utf-8")
for name, anchor in (("UrmIdbOpenFileRead (String", "Idb__FU_OpenFile"),
                     ("UrmIdbOpenBuffer (unsigned char", "Idb__HDR_GetHeader")):
    start = text.index(name)
    end = text.index("\n}\n", start)
    body = text[start:end]
    needle = "*file_id_return = NULL;"
    if needle not in body or body.index(needle) > body.index(anchor):
        raise SystemExit(f"{name} does not initialize its output")
print("PASS Mrm open APIs clear output handles before attempting open")
