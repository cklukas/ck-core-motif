#!/usr/bin/env python3
"""Check Mrm header filter reads data before decommitting the buffer."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIheader.c").read_text(encoding="utf-8")
start = source.index("\nIdb__HDR_MatchFilter (")
region = source[start:]
assert "if (file_id == NULL || data_entry.rec_no != IDBHeaderRecordNumber" in region
assert "if ( ! Idb__HDR_ValidRecord(bufptr) )" in region
assert "matched = TRUE" in region
assert "Idb__BM_Decommit (bufptr);\n  return matched" in region
assert region.index("datahdr->validation") < region.index("Idb__BM_Decommit (bufptr);\n  return matched")
print("Mrm header filter reads data before decommitting its buffer")
