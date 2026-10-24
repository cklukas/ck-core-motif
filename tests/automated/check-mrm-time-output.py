#!/usr/bin/env python3
"""Check Mrm timestamp formatting validates its output pointer."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmtime.c"
text = source.read_text(encoding="utf-8")
start = text.index("Urm__UT_Time (")
end = text.index("\n}\n", start)
region = text[start:end]
guard = region.index("if (time_stg == NULL)")
write = region.index("time (&timeval)")
if guard > write or "return;" not in region[guard:write]:
    raise SystemExit("Mrm timestamp output pointer is not guarded")

print("Mrm timestamp formatting validates its output pointer")
