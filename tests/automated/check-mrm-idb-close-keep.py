#!/usr/bin/env python3
"""Check Mrm IDB close honors keep_new_file for write handles."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmItop.c").read_text(encoding="utf-8")
start = source.index("UrmIdbCloseFile (")
end = source.index("\nUrmIdbGetIndexedResource (", start)
region = source[start:end]
assert "file_id->access == URMWriteAccess && keep_new_file" in region
assert "file_id->access == URMWriteAccess && !keep_new_file" in region
print("Mrm IDB close honors keep_new_file")
