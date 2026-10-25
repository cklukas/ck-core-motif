#!/usr/bin/env python3
"""Check generated parse-table cleanup and substitute ownership."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()
start = source.index("static int\n_get_generate_parse_table (XmParseTable *gen_table)")
end = source.index("\n}\n", start)
function = source[start:end]

assert "if (candidate[0] == NULL || candidate[0]->substitute == NULL)" in function
assert "if (candidate[1] == NULL || candidate[1]->substitute == NULL)" in function
assert function.count("XmParseMappingFree(candidate[0]);") >= 2
assert function.count("XmStringFree(tmp);") >= 2
assert "table = candidate;" in function

print("generated XmString parse-table cleanup is transactional")
