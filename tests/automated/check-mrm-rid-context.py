from pathlib import Path
import os

source = Path(os.environ["MOTIF_SOURCE"])
text = source.read_text()
start = text.index("UrmIdbGetRIDResource (IDBFile")
end = text.index("UrmIdbGetResourceId (IDBFile", start)
body = text[start:end]
assert "if ( context_id == NULL )" in body
assert body.index("if ( context_id == NULL )") < body.index("Idb__FIL_Valid")
print("Mrm RID-resource lookup validates its context")
