#!/usr/bin/env python3
"""Check the shared Mrm buffer-validity predicate handles null pointers."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "IDB.h").read_text(encoding="utf-8")
assert "(bufptr) != NULL && (bufptr)->validation==IDBRecordBufferValid" in source
print("Mrm buffer-validity predicate handles null pointers")
