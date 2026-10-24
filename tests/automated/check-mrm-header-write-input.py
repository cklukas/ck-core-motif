#!/usr/bin/env python3
"""Check Mrm header initialization rejects null metadata inputs."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIheader.c"
text = source.read_text(encoding="utf-8")
start = text.index("Idb__HDR_InitHeader (IDBFile")
init = text.index("Idb__BM_InitRecord", start)
region = text[start:init]
check = region.index("if (file_id == NULL || creator == NULL")
if check > init:
    raise SystemExit("Mrm header writer validates metadata after initialization")
if "creator_version == NULL" not in region or "module_version == NULL" not in region:
    raise SystemExit("Mrm header writer does not validate all metadata strings")
if "return MrmFAILURE;" not in region[check:init]:
    raise SystemExit("Mrm header writer null-input failure is missing")

print("Mrm header initialization validates metadata inputs")
