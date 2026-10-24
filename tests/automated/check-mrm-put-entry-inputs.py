#!/usr/bin/env python3
"""Check Mrm data-entry writes validate outputs and use wide size arithmetic."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIentry.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nIdb__DB_PutDataEntry (")
end = text.find("\n", start + 1)
region = text[start:] if end < 0 else text[start:end]
if "file_id == NULL || data_entry == NULL || ! UrmRCValid (context_id)" not in region:
    raise SystemExit("Mrm data-entry writer lacks file/output validation")
for declaration in ("Cardinal\t\tentsiz", "MrmSize\t\tdatarem", "MrmSize\t\tcursiz"):
    if declaration not in region:
        raise SystemExit(f"Mrm data-entry writer lacks wide size local: {declaration}")
print("Mrm data-entry writer validates inputs and avoids signed size overflow")
