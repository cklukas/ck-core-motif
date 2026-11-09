#!/usr/bin/env python3
"""Check the low-level Mrm file opener validates pointers and length types."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIfile.c"
text = source.read_text(encoding="utf-8")
start = text.index("Idb__FU_OpenFile (char")
end = text.index("\n\n\f", start)
region = text[start:end]
if "size_t\tlength" not in region:
    raise SystemExit("Mrm file-name length is still stored in int")
check = region.index("if (name == NULL || file_id == NULL)")
strlen_pos = region.index("length = strlen (name)")
if check > strlen_pos:
    raise SystemExit("Mrm file opener dereferences inputs before validation")
if "return MrmFAILURE;" not in region[check:strlen_pos]:
    raise SystemExit("Mrm file opener input failure is missing")
if "if (length >= (size_t)UINT_MAX)" not in region:
    raise SystemExit("Mrm file opener does not bound Xt filename allocation")
if "name_bytes = (Cardinal)(length + 1)" not in region:
    raise SystemExit("Mrm file opener does not convert validated allocation size")
if "XtMalloc (name_bytes)" not in region:
    raise SystemExit("Mrm file opener does not pass a validated Cardinal size to XtMalloc")

print("Mrm file opener validates inputs before measuring names")
