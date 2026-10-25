#!/usr/bin/env python3
"""Check explicit conversions at the icon cache's union boundary."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "IconFile.c"
text = source.read_text(encoding="utf-8")
assert "currDir = (DtValidCachedDir)MakeCachedDirEntry(dirName);" in text
assert "cacheList.dirs[cacheList.numDirs++] = (DtCachedDir)currDir;" in text
print("icon cache pointer conversions present")
