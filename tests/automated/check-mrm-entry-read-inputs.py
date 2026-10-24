#!/usr/bin/env python3
"""Check Mrm data-entry readers reject a null file handle before dispatch."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIentry.c"
text = source.read_text(encoding="utf-8")
for name, result in (("Idb__DB_GetDataEntry", "return MrmFAILURE;"),
                     ("Idb__DB_MatchFilter", "return FALSE;")):
    start = text.index("\n" + name + " (")
    end = text.find("\n", start + 1)
    region = text[start:] if end < 0 else text[start:end]
    if "if (file_id == NULL)" not in region:
        raise SystemExit(f"{name} lacks a null file-handle guard")
    guard = region.index("if (file_id == NULL)")
    first_use = region.index("record_number =")
    if guard > first_use:
        raise SystemExit(f"{name} checks file handle too late")
print("Mrm data-entry readers reject null file handles before helper dispatch")
