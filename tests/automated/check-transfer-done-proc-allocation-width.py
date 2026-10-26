#!/usr/bin/env python3
"""Ensure done-callback reallocations use Xt's bounded Cardinal width."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("XmeTransferAddDoneProc(")
end = source.index("\n}\n", start) + 3
body = source[start:end]

assert "allocation_size = sizeof(*new_done_procs) * (size_t)new_count;" in body
assert "XtRealloc((char*) tid -> doneProcs,\n\t\t(Cardinal)allocation_size);" in body
assert "(size_t)new_count > UINT_MAX / sizeof(*new_done_procs)" in body

print("PASS transfer done-callback allocation width is explicit")
