#!/usr/bin/env python3
"""Check Mrm class-registration string allocation sizing."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwci.c"
text = source.read_text(encoding="utf-8")
start = text.index("MrmRegisterClassWithCleanup (")
end = text.index("\n}\n", start)
region = text[start:end]
for needle in (
    "create_name_len = strlen(create_name);",
    "create_name_len == SIZE_MAX",
    "sizeof(WCIClassDesc) > SIZE_MAX - (create_name_len + 1)",
    "create_name_len + 1);",
):
    if needle not in region:
        raise SystemExit(f"Mrm class-registration bound is missing: {needle}")

print("Mrm class-registration string allocation is bounded")
