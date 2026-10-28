#!/usr/bin/env python3
"""Check TextField modify-verify result allocation guards."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("static Boolean \nModifyVerify(")
end = source.index("\nstatic Boolean \n", start + 20)
body = source[start:end]

assert body.count("goto modify_verify_allocation_failed;") >= 4
assert "modify_verify_allocation_failed:" in body
assert "*insert_length = 0;" in body
assert "*free_insert = (int)False;" in body
assert body.count("if (allocated_insert == NULL)") == 4

print("TextField modify-verify allocations are checked")
