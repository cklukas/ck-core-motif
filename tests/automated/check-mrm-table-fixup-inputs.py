from pathlib import Path
import os

source = Path(os.environ.get(
    "MOTIF_SOURCE", Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmtable.c"))
text = source.read_text()
start = text.index("Urm__FixupCompressionTable (UidCompressionTablePtr")
end = text.index("Urm__FindClassDescriptor (IDBFile", start)
block = text[start:end]
assert "ctable == NULL ||" in block
assert "ctable_size < sizeof(UidCompressionTable) - sizeof(UidCTableEntry)" in block
assert "ctable->num_entries < UilMrmMinValidCode" in block
assert "table_bytes > ctable_size" in block
assert "(size_t)ctable->entry[fixndx].stoffset >= ctable_size" in block
assert block.index("ctable->num_entries < UilMrmMinValidCode") < block.index("for ( fixndx=")
print("Mrm compression-table fixup rejects null and undersized tables")
