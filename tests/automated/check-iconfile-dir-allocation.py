#!/usr/bin/env python3
"""Check derived icon-cache directory allocation is transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconFile.c").read_text()
assert "size_t\tnameHeapSize = 0;" in source
assert "nameHeapSize > SIZE_MAX - name_len" in source
assert "nameHeapSize > SIZE_MAX - sizeof(DtValidCachedDirStruct)" in source
assert "(SIZE_MAX - sizeof(DtValidCachedDirStruct) - nameHeapSize)" in source
start = source.index("dirName = (String)XtMalloc")
body = source[start:source.index("iNameEntry->dirName = dirName", start)]
assert "size_t\tdirNameLen;" in body
assert "dirNameLen >= UINT_MAX" in body
assert "XtMalloc((Cardinal)dirNameLen + 1)" in body
assert "if (dirName == NULL)" in body
assert "XtFree(iNameEntry->key_name);" in body
assert "XtFree((char *)iNameEntry);" in body
assert "goto skip_icon_name_cache;" in body
print("PASS icon-file directory allocation is transactional")
