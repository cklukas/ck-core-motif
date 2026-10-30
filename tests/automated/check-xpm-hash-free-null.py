#!/usr/bin/env python3
"""Check Xpm hash cleanup validates a null table before dereferencing it."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmhashtab.c").read_text(encoding="utf-8")
start = source.index("xpmHashTableFree(")
region = source[start:]
assert "if (table == NULL)" in region
assert region.index("if (table == NULL)") < region.index("atomTable = table->atomTable;")
print("PASS Xpm hash cleanup handles a null table")
