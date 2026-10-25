#!/usr/bin/env python3
"""Check that Mrm decommit validates modified record storage."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIbuffer.c"
text = source.read_text(encoding="utf-8")
start = text.index("Idb__BM_Decommit (IDBRecordBufferPtr")
end = text.index("Idb__BM_DecommitAll (IDBFile", start)
body = text[start:end]
assert "buffer->IDB_record == NULL" in body
assert body.index("buffer->IDB_record == NULL") < body.index("Idb__FU_PutBlock")
print("PASS Mrm buffer decommit rejects modified buffers without records")
