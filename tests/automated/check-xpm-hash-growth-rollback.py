#!/usr/bin/env python3
"""Check Xpm hash insertion rolls back when table growth fails."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmhashtab.c").read_text(encoding="utf-8")
start = source.index("xpmHashIntern(")
end = source.index("xpmHashTableInit(", start)
region = source[start:end]
failure = region.index("if ((ErrorStatus = HashTableGrows(table)) != XpmSuccess)")
rollback = region.index("XpmFree(*slot);", failure)
clear = region.index("*slot = NULL;", rollback)
assert failure < rollback < clear
print("PASS Xpm hash insertion rolls back after growth failure")
