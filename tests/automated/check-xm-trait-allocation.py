#!/usr/bin/env python3
"""Check trait registration rolls back failed hash insertion allocations."""
from pathlib import Path

root = Path(__file__).parents[2]
hash_source = (root / "lib" / "Xm" / "Hash.c").read_text()
header = (root / "lib" / "Xm" / "HashI.h").read_text()
trait_source = (root / "lib" / "Xm" / "Trait.c").read_text()
add = hash_source[hash_source.index("_XmAddHashEntry"):hash_source.index("XtPointer\n_XmRemoveHashEntry")]
assert "Boolean\n_XmAddHashEntry" in hash_source
assert "return False;" in add
assert "return True;" in add
assert "Boolean _XmAddHashEntry" in header
set_body = trait_source[trait_source.index("XmeTraitSet"):trait_source.index("static Boolean\nTraitCompare")]
assert "if (entry == NULL)" in set_body
assert "if (!_XmAddHashEntry" in set_body
assert "XtFree((char *)entry);" in set_body
print("PASS Xm trait registration handles hash allocation failure")
