#!/usr/bin/env python3
"""Check Mrm write-open handles the close after creation before reopening."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIfile.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nIdb__FU_OpenFile (")
end = text.find("\n", start + 1)
region = text[start:] if end < 0 else text[start:end]
close_pos = region.index("if (close (file_desc) != 0)")
reopen_pos = region.index("file_desc = open (name, O_RDWR, PMODE);", close_pos)
if close_pos > reopen_pos:
    raise SystemExit("Mrm write-open reopens before checking the create close")
if "(void) unlink (name);" not in region[close_pos:reopen_pos]:
    raise SystemExit("Mrm create-close failure does not clean up the partial file")
print("Mrm write-open checks the create close before reopening")
