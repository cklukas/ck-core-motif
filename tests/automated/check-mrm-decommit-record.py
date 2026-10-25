from pathlib import Path
import os

source = Path(os.environ["MOTIF_SOURCE"])
text = source.read_text()
start = text.index("Idb__BM_Decommit (IDBRecordBufferPtr")
end = text.index("Idb__BM_DecommitAll (IDBFile", start)
body = text[start:end]
assert "buffer->IDB_record == NULL" in body
assert body.index("buffer->IDB_record == NULL") < body.index("Idb__FU_PutBlock")
print("Mrm buffer decommit rejects modified buffers without records")
