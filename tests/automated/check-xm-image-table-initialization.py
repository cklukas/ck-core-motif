#!/usr/bin/env python3
"""Check transactional built-in image-table initialization."""

from pathlib import Path

source_root = Path(__file__).parents[2]
source = (source_root / "lib" / "Xm" / "ImageCache.c").read_text(encoding="utf-8")
hash_source = (source_root / "lib" / "Xm" / "Hash.c").read_text(encoding="utf-8")
start = source.index("static void \nInitializeImageSet")
end = source.index("\n\x0c\n/************************************************************************\n *\n *  _XmInstallImage", start)
body = source[start:end]
assert "XmHashTable new_image_set;" in body
assert "if (image_set != NULL)" in body
assert "ImageData *entries[MAX_BUILTIN_IMAGES]" in body
assert "image_set = new_image_set;" in body
assert "goto allocation_failure;" in body
assert "XtFree((char *)entries[i]);" in body
assert body.index("image_set = new_image_set;") > body.index("_XmAddHashEntry")
print("Xm built-in image tables initialize transactionally")

assert "table->buckets == NULL" in hash_source
assert "return (XmHashTable) NULL;" in hash_source
