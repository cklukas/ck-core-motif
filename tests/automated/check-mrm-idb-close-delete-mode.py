#!/usr/bin/env python3
"""Check Mrm close-error cleanup deletes only write-access files."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmItop.c").read_text(encoding="utf-8")
start = source.index("#define\t_error_close()")
end = source.index("\n\n  if ( (result=Idb__FIL_Valid", start)
region = source[start:end]
assert "file_id->access == URMWriteAccess" in region
assert "Idb__FU_CloseFile(file_id->lowlevel_id,TRUE)" not in region
print("Mrm IDB close-error cleanup preserves read-only files")
