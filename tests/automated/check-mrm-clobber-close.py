#!/usr/bin/env python3
"""Check Mrm clobber-open handles close failure before recreating a file."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIfile.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nIdb__FU_OpenFile (")
end = text.find("\n", start + 1)
region = text[start:] if end < 0 else text[start:end]
needle = "if (close (file_desc) != 0)"
if needle not in region:
    raise SystemExit("Mrm clobber-open does not check the existing close")
close_pos = region.index(needle)
creat_pos = region.index("file_desc = creat", close_pos)
if close_pos > creat_pos:
    raise SystemExit("Mrm clobber-open recreates before checking close")
print("Mrm clobber-open stops when closing the existing descriptor fails")
