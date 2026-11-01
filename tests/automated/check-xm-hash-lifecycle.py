#!/usr/bin/env python3
"""Check Xm hash lookup/removal handles empty and stale iterator state."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Hash.c").read_text()
get = source[source.index("_XmGetHashEntryIterate"):source.index("Boolean\n_XmAddHashEntry")]
add = source[source.index("_XmAddHashEntry"):source.index("XtPointer\n_XmRemoveHashEntry")]
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
assert "if (table->count == UINT_MAX)" in add
assert add.index("if (table->count == UINT_MAX)") < add.index("entry = NewBucket();")
debug = source[source.index("#ifdef DEBUG"):]
assert "if (total_in_use != 0)" in debug
assert "table -> count / total_in_use" in debug
assert "Cardinal i, max, total_in_use;" in debug
assert "Cardinal count;" in debug
assert 'printf("total entries %u\\n", (unsigned int)table -> count);' in debug
print("PASS Xm hash lookup and removal lifecycle is guarded")
