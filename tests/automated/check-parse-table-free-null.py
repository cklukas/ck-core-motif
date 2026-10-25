#!/usr/bin/env python3
"""Check that freeing a missing Xm parse table is a safe no-op."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "XmStringFunc.c"
text = source.read_text(encoding="utf-8")
needle = """  _XmProcessLock();
  if (parse_table == NULL) {
    _XmProcessUnlock();
    return;
  }
  for (i = 0; i < parse_count; i++)
"""
if needle not in text:
    raise SystemExit("parse-table null guard is missing")

print("XmParseTableFree null guard present")
