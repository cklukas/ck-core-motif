from pathlib import Path
import os

source = Path(os.environ.get(
    "MOTIF_SOURCE", Path(__file__).parents[2] / "lib" / "Mrm" / "MrmItop.c"))
text = source.read_text()
start = text.index("UrmIdbGetResourceId (IDBFile")
end = text.index("Idb__FIL_Valid (IDBFile", start)
body = text[start:end]
assert "if ( resource_id_return == NULL )" in body
assert body.index("if ( resource_id_return == NULL )") < body.index("*resource_id_return =")
print("Mrm resource-id allocation validates its output pointer")
