#!/usr/bin/env python3
"""Check Mrm hash-table initialization validates its pointers."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwci.c"
text = source.read_text(encoding="utf-8")
start = text.index("hash_initialize (URMHashTableEntryPtr")
end = text.index("\n}\n", start)
region = text[start:end]
guard = region.index("if (htable == NULL || initflag == NULL)")
deref = region.index("*initflag", region.index("{") + 1)
if guard > deref or "return;" not in region[guard:deref]:
    raise SystemExit("Mrm hash initializer dereferences inputs before validation")

print("Mrm hash initialization validates its pointers")
