#!/usr/bin/env python3
"""Check generation APIs reject an unavailable shared parse table."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
generate_start = source.index("XmString\nXmStringGenerate(")
ungenerate_start = source.index("XtPointer\n_XmStringUngenerate(", generate_start)
generate = source[generate_start:ungenerate_start]
ungenerate = source[ungenerate_start:]
guard = "if (table_size <= 0 || gen_table == NULL)\n    return NULL;"
assert generate.count(guard) == 1
assert ungenerate.count(guard) == 1
print("PASS XmString generation parse-table failure guard")
