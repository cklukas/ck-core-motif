#!/usr/bin/env python3
"""Check deferred Mrm pixmap tag failures release created pixmaps."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
start = text.index("  if ( argptr->tag_code == UilMrmUnknownCode )",
                   text.index("Place resultant Pixmap"))
end = text.index("  *num_used += 1 ;", start)
block = text[start:end]
for needle in (
    "uncmp_res = Urm__UncompressCode",
    "if (uncmp_res != MrmSUCCESS)",
    'Urm__UT_Error ("Urm__CW_CreateArglist"',
    "XFreePixmap (display, pixmap)",
):
    if needle not in block:
        raise SystemExit(f"missing deferred pixmap failure handling: {needle}")
print("PASS Mrm deferred pixmap tag failures are reported and cleaned up")
