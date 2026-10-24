#!/usr/bin/env python3
"""Check that Mrm record initialization validates record type bounds."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIbuffer.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nIdb__BM_InitRecord (")
end = text.find("\n", start + 1)
region = text[start:] if end < 0 else text[start:end]
needle = "if (type < IDBrtMin || type > IDBrtMax)"
if needle not in region:
    raise SystemExit("Idb__BM_InitRecord lacks record-type range validation")
check = region.index(needle)
get_buffer = region.index("Idb__BM_GetBuffer")
if check > get_buffer:
    raise SystemExit("record-type validation occurs after buffer acquisition")
print("Mrm record initialization validates type bounds before buffer acquisition")
