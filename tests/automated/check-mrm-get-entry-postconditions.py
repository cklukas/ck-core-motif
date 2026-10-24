#!/usr/bin/env python3
"""Check Mrm data-entry reads validate buffer-manager postconditions."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIentry.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nIdb__DB_GetDataEntry (")
end = text.find("\n", start + 1)
region = text[start:] if end < 0 else text[start:end]
needle = "if (curbuf == NULL || curbuf->IDB_record == NULL)"
if region.count(needle) < 2:
    raise SystemExit("Mrm data-entry reader lacks buffer postcondition checks")
print("Mrm data-entry reader checks initial and overflow-chain buffers")
