#!/usr/bin/env python3
"""Check DataField class binding assembly handles allocation failure."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/DataF.c").read_text(
    encoding="utf-8"
)
start = source.index("df_ClassPartInitialize(")
start = source.index("event_bindings =", start)
end = source.index("strcpy(event_bindings", start)
context = source[start:end]
if "if (event_bindings == NULL)" not in context or "return;" not in context:
    raise SystemExit("class binding allocation is not guarded")

print("DataField class-binding checks passed")
