#!/usr/bin/env python3
"""Check callback fixup validates descriptors and conversion results."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
start = text.rfind("\nUrm__CW_FixupCallback (")
end = text.find("\n\n\f", start)
region = text[start:end]
for needle in (
    "cbdesc == NULL || bufptr == NULL",
    "cbdesc->validation != URMCallbackDescriptorValid",
    "cbdesc->count > RGMListSizeMax",
    "if ( resptr == NULL )",
    "if ( result != MrmSUCCESS ) continue ;\n\t  Urm__CW_SafeCopyValue",
):
    if needle not in region:
        raise SystemExit(f"missing callback fixup guard: {needle}")
print("PASS Mrm callback fixup validates descriptors and conversion results")
