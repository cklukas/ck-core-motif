#!/usr/bin/env python3
"""Check icon-directory cache growth and entry allocation failure handling."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconFile.c").read_text()
cache = source[source.index("CheckDirCache"):source.index("static Boolean\nTestIconFile")]
entry = source[source.index("MakeCachedDirEntry"):source.index("DtCachedDirListStruct cacheList")]
assert "DtCachedDir *new_dirs" in cache
assert "new_dirs = (DtCachedDir *)XtRealloc" in cache
assert "if (new_dirs == NULL)" in cache
assert "cacheList.dirs = new_dirs" in cache
assert "if (dirName == NULL)" in cache
assert "if (new_dir == NULL)" in cache
assert "if (validDir == NULL)" in entry
assert "if (cachedDir == NULL)" in entry
assert "#include <limits.h>" in source
print("PASS icon-directory cache growth handles allocation failure")
