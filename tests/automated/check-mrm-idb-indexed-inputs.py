from pathlib import Path
import os

source = Path(os.environ.get(
    "MOTIF_SOURCE", Path(__file__).parents[2] / "lib" / "Mrm" / "MrmItop.c"))
text = source.read_text()
start = text.index("UrmIdbGetIndexedResource (IDBFile")
end = text.index("UrmIdbFindIndexedResource (IDBFile", start)
body = text[start:end]
assert "index == NULL || context_id == NULL" in body
assert body.index("index == NULL || context_id == NULL") < body.index("Idb__FIL_Valid")
print("Mrm indexed-resource lookup rejects null index and context")
