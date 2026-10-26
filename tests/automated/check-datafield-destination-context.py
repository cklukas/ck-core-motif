#!/usr/bin/env python3
"""Check destination-context allocations before initialization/registration."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/DataF.c").read_text(
    encoding="utf-8"
)

start = source.rindex("static TextFDestData")
end = source.index("df_SetDropContext", start)
context = source[start:end]
required = (
    "if (ctx_data == NULL)",
    "if (dest_data == NULL)",
    "XtFree((char *)ctx_data);",
)
missing = [item for item in required if item not in context]
if missing:
    raise SystemExit("destination-context guard missing: " + ", ".join(missing))

print("DataField destination-context checks passed")
