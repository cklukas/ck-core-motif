from pathlib import Path
import os

source = Path(os.environ["MOTIF_SOURCE"])
text = source.read_text()
find = text[text.index("Urm__FindClassDescriptor (IDBFile"):text.index("Urm__UncompressCode (IDBFile")]
uncompress = text[text.index("Urm__UncompressCode (IDBFile"):text.index("Urm__IsSubtreeResource (IDBFile")]
subtree = text[text.index("Urm__IsSubtreeResource (IDBFile"): ]
assert "class_return == NULL" in find and "cfile == NULL" in find
assert "cfile == NULL || stg_return == NULL" in uncompress
assert "*stg_return == NULL" in uncompress
assert "cfile == NULL || cfile->resource_ctable == NULL" in subtree
assert "entry[code].cstring == NULL" in subtree
print("Mrm compression lookups reject null inputs and entries")
