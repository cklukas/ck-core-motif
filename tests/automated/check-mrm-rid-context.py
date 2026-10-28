from pathlib import Path
import os

source = Path(os.environ.get(
    "MOTIF_SOURCE", Path(__file__).parents[2] / "lib" / "Mrm" / "MrmItop.c"))
text = source.read_text()
start = text.index("UrmIdbGetRIDResource (IDBFile")
end = text.index("UrmIdbGetResourceId (IDBFile", start)
body = text[start:end]
assert "if ( context_id == NULL )" in body
assert body.index("if ( context_id == NULL )") < body.index("Idb__FIL_Valid")
print("Mrm RID-resource lookup validates its context")
