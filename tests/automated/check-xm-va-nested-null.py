#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/VaSimple.c").read_text()

count_start = source.index("_XmCountNestedList(", source.index("_XmCountNestedList(") + 1)
count_region = source[count_start:source.index("\n}\n", count_start) + 2]
assert "if (avlist == NULL || total_count == NULL || typed_count == NULL)" in count_region

convert_start = source.index("_XmNestedArgtoArg(", source.index("_XmNestedArgtoArg(") + 1)
convert_region = source[convert_start:source.index("\n}\n", convert_start) + 2]
assert "if (avlist == NULL || args == NULL)" in convert_region

print("Nested Motif varargs helpers ignore null lists safely")
