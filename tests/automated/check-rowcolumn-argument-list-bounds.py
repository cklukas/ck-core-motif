#!/usr/bin/env python3
"""Check RowColumn menu creation does not use fixed argument-list stacks."""
from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/RowColumn.c").read_text(
    encoding="utf-8"
)
start = source.index("static Widget \ncreate(")
end = source.index("/*\n *************************************************************************\n *\n * Public Routines", start)
region = source[start:end]

assert "Arg al[256]" not in region
assert "Arg s_al[256]" not in region
assert "old_ac != 0 && old_al == NULL" in region
assert "old_ac > (Cardinal)(~(Cardinal)0) - add_count" in region
assert "al_bytes / sizeof(*al) != (size_t)ac || al_bytes > UINT_MAX" in region
assert "al = (Arg *)XtMalloc((Cardinal)al_bytes);" in region
assert "old_ac > (Cardinal)(~(Cardinal)0) - shell_add_count" in region
assert "s_al_bytes / sizeof(*s_al) != (size_t)s_ac ||" in region
assert "s_al = (Arg *)XtMalloc((Cardinal)s_al_bytes);" in region
assert "XtFree((char *)s_al);" in region
assert "XtFree((char *)al);" in region
print("PASS RowColumn menu creation bounds dynamic argument lists")
