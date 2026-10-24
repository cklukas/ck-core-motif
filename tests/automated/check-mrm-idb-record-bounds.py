#!/usr/bin/env python3
"""Check Mrm in-memory record access validates record bounds."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIbuffer.c").read_text(encoding="utf-8")
start = source.index("Idb__BM_GetRecord (")
end = source.index("\n}\n", start)
region = source[start:end]
assert "record <= 0" in region
assert "record != IDBHeaderRecordNumber" in region
assert "record > file_id->last_record" in region
assert region.index("record <= 0") < region.index("buf_src = file_id->uid_buffer")
print("Mrm in-memory record access validates record bounds")
