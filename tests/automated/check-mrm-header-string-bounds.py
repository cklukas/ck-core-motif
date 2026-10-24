#!/usr/bin/env python3
"""Check Mrm UID header strings are copied only when NUL-terminated."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIheader.c"
text = source.read_text(encoding="utf-8")
helper = text.index("Idb__HDR_CopyField")
copy_block = text.index("if (Idb__HDR_CopyField", helper)
end = text.index("\n  /*\n   * Header successfully read", copy_block)
region = text[helper:end]
if "memchr(source, '\\0', source_size)" not in region:
    raise SystemExit("Mrm header copy does not bound the source field")
if "memcpy(destination, source, length + 1)" not in region:
    raise SystemExit("Mrm header copy does not copy a checked field")
if "strcpy (file_id->" in region:
    raise SystemExit("unbounded Mrm header field copy remains")
if region.count("Idb__HDR_CopyField(") != 6:
    raise SystemExit("not all six Mrm header fields use the checked copier")
if "MrmNOT_VALID" not in region:
    raise SystemExit("malformed Mrm header fields are not rejected")

print("Mrm UID header strings validate bounded termination")
