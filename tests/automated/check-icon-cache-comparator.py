#!/usr/bin/env python3
"""Check IconFile cache comparison validates entries before strcmp."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconFile.c").read_text()
start = source.rindex("CompareIconNames (XmHashKey key_1")
end = source.index("\n}\n", start) + 2
function = source[start:end]

assert "data_1 != NULL && data_2 != NULL" in function
assert "data_1->key_name != NULL && data_2->key_name != NULL" in function
assert function.index("data_1 != NULL") < function.index("strcmp")
print("PASS IconFile cache comparator input guards")
