#!/usr/bin/env python3
"""Check Mrm IDB decommit detaches every buffer after an error."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIbuffer.c").read_text(encoding="utf-8")
start = source.index("Idb__BM_DecommitAll (")
end = source.index("\n}\n", start)
region = source[start:end]
assert "first_error = MrmSUCCESS" in region
assert "curbuf->cur_file = NULL" in region
assert "return first_error" in region
assert "return result" not in region
print("Mrm IDB decommit detaches all buffers after an error")
