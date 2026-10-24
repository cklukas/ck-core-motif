#!/usr/bin/env python3
"""Check Mrm decommit-all rejects a missing file handle."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIbuffer.c").read_text(encoding="utf-8")
start = source.index("\nIdb__BM_DecommitAll (")
end = source.find("\nCardinal ", start + 1)
region = source[start:] if end < 0 else source[start:end]
assert "if (file_id == NULL)" in region
assert "return MrmFAILURE;" in region
print("Mrm decommit-all validates its file handle")
