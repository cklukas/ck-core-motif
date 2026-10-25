#!/usr/bin/env python3
"""Check Mrm file-open read rejects a null filename."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmItop.c"
text = source.read_text(encoding="utf-8")
start = text.index("UrmIdbOpenFileRead (String")
end = text.index("UrmIdbOpenBuffer (unsigned char", start)
body = text[start:end]
needle = "name == NULL || file_id_return == NULL"
if needle not in body or body.index(needle) > body.index("Idb__FU_OpenFile"):
    raise SystemExit("Mrm file-open read lacks filename guard")
print("PASS Mrm file-open read rejects a null filename")
