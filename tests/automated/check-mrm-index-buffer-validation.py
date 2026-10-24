#!/usr/bin/env python3
"""Check Mrm index writers validate record buffers before dereferencing."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIindexw.c").read_text(encoding="utf-8")
assert "(buffer) != NULL && (buffer)->IDB_record != NULL" in source
for function in ("Idb__INX_EnterLeafIndex", "Idb__INX_EnterNodeIndex"):
    start = source.index("\n" + function + " (")
    end = source.find("\nCardinal ", start + 1)
    region = source[start:] if end < 0 else source[start:end]
    assert "buffer->IDB_record == NULL" in region, function
start = source.index("\nIdb__INX_FixNodeChildren (")
end = source.find("\n/*", start + 1)
region = source[start:] if end < 0 else source[start:end]
assert "file_id == NULL || p_record <= 0" in region
print("Mrm index writers validate record buffers and parent inputs")
