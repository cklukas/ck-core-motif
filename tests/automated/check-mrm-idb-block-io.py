#!/usr/bin/env python3
"""Check Mrm IDB block I/O completes short and interrupted transfers."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIfile.c").read_text(encoding="utf-8")
for function, counter, syscall in (
    ("Idb__FU_GetBlock", "bytes_read", "read ("),
    ("Idb__FU_PutBlock", "bytes_written", "write ("),
):
    start = source.index(function + " (")
    end = source.index("\n}\n", start)
    region = source[start:end]
    assert f"while ({counter} < IDBRecordSize)" in region, function
    assert syscall in region, function
    assert "errno == EINTR" in region, function
print("Mrm IDB block I/O completes short and interrupted transfers")
