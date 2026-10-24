#!/usr/bin/env python3
"""Check that Mrm cache-hit scans validate record storage before dereference."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIbuffer.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nIdb__BM_GetRecord (")
end = text.find("\nCardinal ", start + 1)
region = text[start:] if end < 0 else text[start:end]
needle = "curbuf->IDB_record != NULL"
if needle not in region:
    raise SystemExit("Mrm cache-hit scan does not validate record storage")
print("Mrm cache-hit scan validates record storage before header access")
