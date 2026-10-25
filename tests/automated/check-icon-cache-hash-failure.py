#!/usr/bin/env python3
"""Check IconFile cache hashing width and insertion rollback."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconFile.c").read_text()
hash_start = source.rindex("HashIconName (XmHashKey key)")
hash_end = source.index("\n}\n", hash_start) + 2
hash_body = source[hash_start:hash_end]
assert "size_t len;" in hash_body
assert "uintmax_t" in hash_body
assert "unsigned int len" not in hash_body

insert_start = source.index("if (iconNameCache != NULL)")
insert = source[insert_start:source.index("skip_icon_name_cache:", insert_start)]
assert "if (!_XmAddHashEntry(iconNameCache" in insert
assert "XtFree(iNameEntry->leafName);" in insert
assert "XtFree(iNameEntry->dirName);" in insert
assert "XtFree(iNameEntry->key_name);" in insert
print("PASS IconFile cache hash and insertion failure handling")
