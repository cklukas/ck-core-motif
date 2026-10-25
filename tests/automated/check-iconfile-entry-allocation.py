#!/usr/bin/env python3
"""Check icon-file cache entry allocation failure skips optional caching."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconFile.c").read_text()
start = source.index("/** alloc a icon cache entry **/")
body = source[start:source.index("skip_icon_name_cache:", start)]
assert "iNameEntry = (DtIconNameEntry) XtMalloc(sizeof(DtIconNameEntryRec));" in body
assert "if (iNameEntry == NULL)" in body
assert "goto skip_icon_name_cache;" in body
assert "iNameEntry->key_name" in body
print("PASS icon-file cache entry allocation is guarded")
