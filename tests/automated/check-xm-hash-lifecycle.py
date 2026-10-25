#!/usr/bin/env python3
"""Check Xm hash lookup/removal handles empty and stale iterator state."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Hash.c").read_text()
get = source[source.index("_XmGetHashEntryIterate"):source.index("Boolean\n_XmAddHashEntry")]
remove = source[source.index("_XmRemoveHashEntry"):source.index("XtPointer \n_XmRemoveHashIterator")]
iterator = source[source.index("_XmRemoveHashIterator"):source.index("Cardinal\n_XmHashTableCount")]
map_body = source[source.index("_XmMapHashTable"):source.index("/* Bucket management */")]
assert "table == NULL || table->buckets == NULL || table->size == 0" in get
assert "XtPointer key_return" in remove
assert "key_return = entry -> hash_key" in remove
assert "FreeBucket(entry);\n      return key_return;" in remove
assert "*iterator = NULL" in iterator
assert "XtPointer key_return" in iterator
assert "FreeBucket(current);\n      *iterator = NULL" in iterator
assert "proc == NULL" in map_body
assert "return table ? table -> count : 0;" in source
assert "return table ? table -> size : 0;" in source
print("PASS Xm hash lookup and removal lifecycle is guarded")
