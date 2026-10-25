#!/usr/bin/env python3
"""Check that in-memory Mrm reads reject null source buffers."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIbuffer.c"
text = source.read_text(encoding="utf-8")
start = text.index("Idb__BM_GetRecord (IDBFile")
end = text.index("Idb__BM_InitRecord (IDBFile", start)
body = text[start:end]
assert "file_id->uid_buffer == NULL" in body
assert "record != IDBHeaderRecordNumber" in body
assert body.index("file_id->uid_buffer == NULL") < body.index("record != IDBHeaderRecordNumber")
print("PASS Mrm in-memory record reads reject null source buffers")
