#!/usr/bin/env python3
"""Check Xm hash buckets and resize replacement are allocation-safe."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Hash.c").read_text()
assert "#include <stdint.h>" in source
assert "if (table == NULL)" in source[source.index("_XmFreeHashTable"):source.index("_XmResizeHashTable")]
resize = source[source.index("_XmResizeHashTable"):source.index("XtPointer\n_XmGetHashEntryIterate")]
assert "XmHashBucket *new_buckets" in resize
assert "XmHashBucket *old_buckets" in resize
assert "if (new_buckets == NULL)" in resize
assert "memcpy(new_buckets" in resize
assert "XtFree((char *)old_buckets);" in resize
add = source[source.index("_XmAddHashEntry"):source.index("XtPointer\n_XmRemoveHashEntry")]
assert "if (entry == NULL)" in add
new_bucket = source[source.index("NewBucket(void)"):source.index("static void\nFreeBucket")]
assert "if (buckets == NULL)" in new_bucket
assert "i < NUMBUCKETS - 1" in new_bucket
print("PASS Xm hash allocation and bucket initialization are guarded")
