from pathlib import Path
import os

source = Path(os.environ["MOTIF_SOURCE"])
text = source.read_text()
start = text.index("Urm__FixupCompressionTable (UidCompressionTablePtr")
end = text.index("Urm__FindClassDescriptor (IDBFile", start)
block = text[start:end]
assert "size_t table_bytes" in block
assert "SIZE_MAX" in block
assert "ctable->entry[fixndx].stoffset < table_bytes" in block
assert block.index("ctable->entry[fixndx].stoffset < table_bytes") < block.index("ctable->entry[fixndx].cstring")
print("Mrm compression-table offsets stay beyond the table header")
