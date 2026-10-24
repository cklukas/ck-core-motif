#!/usr/bin/env python3
"""Check Mrm widget-reference accounting and growth guards."""

from pathlib import Path

root = Path(__file__).parents[2]
header = (root / "lib" / "Mrm" / "Mrm.h").read_text(encoding="utf-8")
source = (root / "lib" / "Mrm" / "Mrmwrefs.c").read_text(encoding="utf-8")

assert "(ref_id)->num_refs > 0" in header
assert "bytes_used > SIZE_MAX - bytes_needed" in source
assert "while (new_size < bytes_used + bytes_needed)" in source
assert "new_size > (size_t)INT_MAX" in source
print("Mrm widget-reference growth accounts for empty and multi-step buffers")
