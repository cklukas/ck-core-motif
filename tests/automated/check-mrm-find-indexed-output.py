from pathlib import Path
import os

source = Path(os.environ["MOTIF_SOURCE"])
text = source.read_text()
start = text.index("UrmIdbFindIndexedResource (IDBFile")
end = text.index("UrmIdbGetRIDResource (IDBFile", start)
body = text[start:end]
assert "if ( index_list == NULL )" in body
assert body.index("if ( index_list == NULL )") < body.index("Idb__FIL_Valid")
print("Mrm indexed-resource enumeration validates its output list")
