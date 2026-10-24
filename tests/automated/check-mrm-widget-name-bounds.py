#!/usr/bin/env python3
"""Check Mrm widget-name lookup sizing and copying are overflow-safe."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
start = text.index("UrmSetWidgetInstance (")
end = text.index("\n}\n", start)
region = text[start:end]
for needle in (
    "c_name_len = strlen(c_name);",
    "c_name_len > SIZE_MAX - 2",
    "c_name_tmp[0] = '*';",
    "memcpy(c_name_tmp + 1, c_name, c_name_len + 1);",
):
    if needle not in region:
        raise SystemExit(f"Mrm widget-name bound is missing: {needle}")
if 'sprintf(c_name_tmp, "*%s", c_name)' in region:
    raise SystemExit("unbounded widget-name formatting remains")

print("Mrm widget-name lookup sizing is bounded")
