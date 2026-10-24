#!/usr/bin/env python3
"""Check Mrm hash-table callers handle insertion failure."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwci.c"
text = source.read_text(encoding="utf-8")
registration = text[text.index("MrmRegisterClassWithCleanup ("):]
if "if (hash_entry == NULL)" not in registration[:registration.index("\n}\n")]:
    raise SystemExit("class registration does not handle hash insertion failure")
names = text[text.index("Urm__WCI_RegisterNames ("):]
names = names[:names.index("\n}\n")]
for needle in (
    "names == NULL || values == NULL",
    "if (hash_entry == NULL)",
):
    if needle not in names:
        raise SystemExit(f"resource-name registration guard is missing: {needle}")

print("Mrm hash-table callers handle insertion failure")
