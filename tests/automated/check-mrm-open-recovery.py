#!/usr/bin/env python3
"""Check Mrm write-open recovery removes a partially created file."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIfile.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nIdb__FU_OpenFile (")
end = text.find("\n", start + 1)
region = text[start:] if end < 0 else text[start:end]
open_pos = region.index("file_desc = open (name, O_RDWR, PMODE);")
fail_pos = region.index("if (file_desc == FAILURE)", open_pos)
unlink_pos = region.index("unlink (name)", fail_pos)
if unlink_pos < fail_pos:
    raise SystemExit("Mrm write-open cleanup occurs before reopen failure")
print("Mrm write-open recovery removes a partial file after reopen failure")
