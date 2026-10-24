#!/usr/bin/env python3
"""Check the writable database-version copy handles allocation failure."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIheader.c"
text = source.read_text(encoding="utf-8")
start = text.index("/* sscanf() may call ungetc()")
region = text[start : text.index("  }", start) + 3]

allocation = "char *buf = XtMalloc(strlen(idb__database_version) + 1);"
if allocation not in region:
    raise SystemExit("database-version allocation is missing")
if "if (buf == NULL)" not in region:
    raise SystemExit("database-version allocation has no null guard")
if "_MrmMMsg_0001" not in region or "MrmFAILURE" not in region:
    raise SystemExit("database-version allocation failure does not return MrmFAILURE")
if region.index("if (buf == NULL)") > region.index("strcpy(buf"):
    raise SystemExit("database-version buffer is copied before the null guard")

print("Mrm header version allocation is checked")
