#!/usr/bin/env python3
"""Check Mrm decommit resets buffer state for all access modes."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIbuffer.c").read_text(encoding="utf-8")
start = source.index("\nIdb__BM_Decommit (")
end = source.find("\nCardinal ", start + 1)
region = source[start:] if end < 0 else source[start:end]
assert "buffer->cur_file == NULL" in region
assert "buffer->activity = 0;" in region
assert "buffer->modified = FALSE;" in region
assert region.index("buffer->activity = 0;") > region.index("if ((buffer->access")
print("Mrm decommit resets state and rejects detached writes")
