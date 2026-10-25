from pathlib import Path
import os

source = Path(os.environ["MOTIF_SOURCE"])
text = source.read_text()
start = text.index("Idb__BM_GetRecord (IDBFile")
end = text.index("Idb__BM_InitRecord (IDBFile", start)
body = text[start:end]
assert "file_id->uid_buffer == NULL" in body
assert "record != IDBHeaderRecordNumber" in body
assert body.index("file_id->uid_buffer == NULL") < body.index("record != IDBHeaderRecordNumber")
print("Mrm in-memory record reads reject null source buffers")
