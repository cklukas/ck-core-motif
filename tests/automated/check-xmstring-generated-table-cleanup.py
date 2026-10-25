#!/usr/bin/env python3
"""Check generated parse-table cleanup and substitute ownership."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()
start = source.index("static int\n_get_generate_parse_table (XmParseTable *gen_table)")
end = source.index("\n}\n", start)
function = source[start:end]

assert "if (table[0] == NULL)" in function
assert "if (table[1] == NULL)" in function
assert function.count("XmParseMappingFree(table[0]);") == 1
assert function.count("XmStringFree(tmp);") >= 2
assert "table = NULL;" in function

print("generated XmString parse-table cleanup is transactional")
