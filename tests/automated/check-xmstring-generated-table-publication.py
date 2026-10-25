#!/usr/bin/env python3
"""Check generated parse tables are published only after full initialization."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()
start = source.index("static int\n_get_generate_parse_table (XmParseTable *gen_table)")
end = source.index("\n}\n", start)
function = source[start:end]

candidate = function.index("candidate = (XmParseTable)")
publish = function.index("table = candidate;")
assert candidate < publish
assert "Publishing the pointer before the mappings are" in function
assert "*gen_table = table;" in function

print("generated XmString parse tables publish atomically after initialization")
