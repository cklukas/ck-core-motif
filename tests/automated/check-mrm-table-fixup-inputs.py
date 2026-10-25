from pathlib import Path
import os

source = Path(os.environ["MOTIF_SOURCE"])
text = source.read_text()
start = text.index("Urm__FixupCompressionTable (UidCompressionTablePtr")
end = text.index("Urm__FindClassDescriptor (IDBFile", start)
block = text[start:end]
assert "if ( ctable == NULL )" in block
assert "ctable->num_entries < UilMrmMinValidCode" in block
assert block.index("ctable->num_entries < UilMrmMinValidCode") < block.index("for ( fixndx=")
print("Mrm compression-table fixup rejects null and undersized tables")
