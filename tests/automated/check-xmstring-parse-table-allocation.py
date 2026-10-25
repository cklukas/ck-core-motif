#!/usr/bin/env python3
"""Check lazy XmString parse-table initialization handles allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.rindex("_get_generate_parse_table")
end = source.index("/* Destructively truncates str", start)
region = source[start:end]
assert "if (gen_table == NULL)" in region
assert "if (table == NULL)" in region
assert "*gen_table = NULL;" in region
alloc = region.index("table = (XmParseTable)")
assert alloc < region.index("if (table == NULL)", alloc)
assert region.index("if (table == NULL)", alloc) < region.index("*gen_table = table;", alloc)
print("PASS XmString lazy parse-table allocation is guarded")
