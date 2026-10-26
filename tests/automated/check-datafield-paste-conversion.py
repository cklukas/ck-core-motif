#!/usr/bin/env python3
"""Check DataField paste conversion bounds and allocation failures."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/DataF.c").read_text(
    encoding="utf-8"
)

required = (
    "size_t malloc_size = 0;",
    "malloc_size + item_length > (size_t)UINT_MAX",
    "if (total_tmp_value == NULL)",
    "malloc_size * sizeof(wchar_t) > (size_t)UINT_MAX",
    "if (wc_value == NULL)",
    "converted == (size_t)-1",
)
missing = [item for item in required if item not in source]
if missing:
    raise SystemExit("paste conversion guard missing: " + ", ".join(missing))

print("DataField paste-conversion checks passed")
