#!/usr/bin/env python3
"""Check transactional transfer completion-callback growth."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("XmeTransferAddDoneProc(")
end = source.index("\nXmePrimarySource(", start)
region = source[start:end]

assert "XmSelectionFinishedProc *new_done_procs;" in region
assert "if (tid == NULL)" in region
assert "if (tid -> numDoneProcs == INT_MAX)" in region
assert "(size_t)new_count > SIZE_MAX / sizeof(*new_done_procs)" in region
assert "(size_t)new_count > UINT_MAX / sizeof(*new_done_procs)" in region
assert "allocation_size = sizeof(*new_done_procs) * (size_t)new_count" in region
assert "new_done_procs = (XmSelectionFinishedProc *)" in region
assert "if (new_done_procs == NULL)" in region
assert "tid -> numDoneProcs = new_count;" in region
assert "tid -> doneProcs[new_count - 1] = done_proc;" in region

print("PASS transfer completion-callback growth checks allocation failures")
