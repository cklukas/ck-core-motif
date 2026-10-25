#!/usr/bin/env python3
"""Check generated parse-table substitute validation."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()
start = source.index("static int\n_get_generate_parse_table (XmParseTable *gen_table)")
end = source.index("\n}\n", start)
function = source[start:end]

assert "if (tmp == NULL)" in function
assert "candidate[0]->substitute == NULL" in function
assert "candidate[1]->substitute == NULL" in function
assert function.count("XmParseMappingFree(candidate[0]);") >= 2

print("generated XmString parse-table substitutes are validated")
