#!/usr/bin/env python3
"""Ensure transfer completion never queues a null callback."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("XmeTransferAddDoneProc(")
body = source[start:source.index("\n}\n", start) + 3]
assert "if (tid == NULL || done_proc == NULL)" in body
assert "tid -> doneProcs[new_count - 1] = done_proc;" in body

print("PASS transfer completion rejects null done callbacks")
