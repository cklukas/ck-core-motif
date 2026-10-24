#!/usr/bin/env python3
"""Check that Mrm filter matching reads a record before decommitting it."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIentry.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nIdb__DB_MatchFilter (")
region = text[start:]
first_decommit = region.index("Idb__BM_Decommit (bufptr)")
record_read = region.index("data_rec = (IDBDataRecord *) bufptr->IDB_record")
if first_decommit < record_read:
    raise SystemExit("Mrm filter matching decommits before reading the record")
if region.count("Idb__BM_Decommit (bufptr)") < 3:
    raise SystemExit("Mrm filter matching does not release the buffer on all exits")
print("Mrm filter matching keeps the record active until filtering completes")
