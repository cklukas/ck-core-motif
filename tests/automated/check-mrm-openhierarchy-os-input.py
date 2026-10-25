#!/usr/bin/env python3
"""Check Mrm hierarchy open rejects null OS extension objects."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmhier.c"
text = source.read_text(encoding="utf-8")
start = text.index("Urm__OpenHierarchy (MrmCount")
end = text.index("#ifndef NO_MESSAGE_CATALOG", start)
block = text[start:end]
needle = "os_ext_list != NULL && *os_ext_list == NULL"
if needle not in block or block.index(needle) > block.index("return MrmFAILURE"):
    raise SystemExit("Mrm hierarchy open lacks OS extension input guard")
print("PASS Mrm hierarchy open rejects null OS extension objects")
