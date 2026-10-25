#!/usr/bin/env python3
"""Check representation-type copies roll back partial allocations."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "RepType.c"
text = source.read_text()
copy_start = text.index("CopyStringArray(")
copy_end = text.index("static void\nFreeCopiedRecord", copy_start)
copy_block = text[copy_start:copy_end]
record_start = text.index("CopyRecord(")
record_end = text.index("static Boolean\nValuesConsecutiveStartingAtZero", record_start)
record_block = text[record_start:record_end]

for needle in ("if (TmpStr == NULL)", "goto failure;", "XtFree((char *)TmpStr);"):
    if needle not in copy_block:
        raise SystemExit(f"string-array rollback is missing {needle}")
for needle in ("if (OutputEntry->value_names == NULL)",
               "FreeCopiedRecord(OutputEntry);",
               "return FALSE;"):
    if needle not in record_block:
        raise SystemExit(f"record-copy rollback is missing {needle}")

print("representation-type string and record copies roll back failures")
