#!/usr/bin/env python3
"""Check Mrm close failure still attempts requested deletion."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIfile.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nIdb__FU_CloseFile (")
end = text.find("\n", start + 1)
region = text[start:] if end < 0 else text[start:end]
close_fail = region.index("if (status != 0)")
unlink = region.index("unlink (file_id->name)", close_fail)
if "if (delete)" not in region[close_fail:unlink]:
    raise SystemExit("Mrm close failure does not honor delete")
print("Mrm close failure attempts deletion when requested")
