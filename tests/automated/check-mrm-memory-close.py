from pathlib import Path
import os

root = Path(__file__).parents[2] / "lib" / "Mrm"
itop = Path(os.environ.get("MOTIF_ITOP_SOURCE", root / "MrmItop.c")).read_text()
hier = Path(os.environ.get("MOTIF_HIER_SOURCE", root / "Mrmhier.c")).read_text()
start = itop.index("UrmIdbCloseFile (IDBFile")
end = itop.index("UrmIdbGetIndexedResource (IDBFile", start)
close = itop[start:end]
assert "if ( !file_id->in_memory )" in close, "error cleanup must not close a null low-level handle"
assert "if ( file_id->in_memory )" in close, "memory-backed files need a successful close path"
assert "result = MrmSUCCESS" in close
hstart = hier.index("Urm__CloseHierarchy (MrmHierarchy")
hend = hier.index("UrmHGetIndexedResource (MrmHierarchy", hstart)
hclose = hier[hstart:hend]
assert "if (hierarchy_id->file_list[ndx]->in_memory == FALSE )" not in hclose
assert "UrmIdbCloseFile (hierarchy_id->file_list[ndx], FALSE)" in hclose
print("Mrm memory-backed files close successfully and hierarchies release them")
