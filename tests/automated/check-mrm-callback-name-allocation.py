#!/usr/bin/env python3
"""Check Mrm deferred callback widget names handle allocation failure."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
start = text.index("Cardinal \nUrm__CW_FixupCallback (")
end = text.find("\n\n\f", start)
region = text[start:] if end < 0 else text[start:end]
marker = "itmptr->runtime.wname = Urm__UT_AllocString(ref_name);"
alloc = region.index(marker)
guard = region.find("if (itmptr->runtime.wname == NULL)", alloc)
if guard < 0 or guard - alloc > 180:
    raise SystemExit("deferred callback widget-name allocation is unchecked")
error = region.find("_MrmMMsg_0001", guard)
if error < 0 or error - guard > 220:
    raise SystemExit("deferred callback widget-name allocation lacks failure reporting")
completed = region.find("tag_val = 0L;", alloc)
if completed < 0 or guard > completed:
    raise SystemExit("callback fixup publishes deferred state before checking allocation")

print("Mrm deferred callback widget-name allocation failure propagates")
