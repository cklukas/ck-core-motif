#!/usr/bin/env python3
"""Check memory-backed Mrm files close without filesystem handles."""
from pathlib import Path

itop = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmItop.c").read_text(
    encoding="utf-8")
hier = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmhier.c").read_text(
    encoding="utf-8")
start = itop.index("UrmIdbCloseFile (IDBFile")
end = itop.index("UrmIdbGetIndexedResource (IDBFile", start)
close = itop[start:end]
if "if ( !file_id->in_memory )" not in close:
    raise SystemExit("error cleanup still closes a null in-memory handle")
if "if ( file_id->in_memory )" not in close or "result = MrmSUCCESS" not in close:
    raise SystemExit("memory-backed close lacks success path")
hstart = hier.index("Urm__CloseHierarchy (MrmHierarchy")
hend = hier.index("UrmHGetIndexedResource (MrmHierarchy", hstart)
hclose = hier[hstart:hend]
if "in_memory == FALSE" in hclose or "UrmIdbCloseFile (hierarchy_id->file_list[ndx], FALSE)" not in hclose:
    raise SystemExit("hierarchy cleanup does not close memory-backed files")
print("PASS Mrm memory-backed files close successfully and hierarchies release them")
