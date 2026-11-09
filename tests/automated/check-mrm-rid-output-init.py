#!/usr/bin/env python3
"""Check direct RID allocators clear a supplied output before failure."""

from pathlib import Path


checks = (
    ("MrmIheader.c", "Idb__HDR_NextRID ("),
    ("MrmIrid.c", "Idb__RID_NextRID ("),
)
root = Path(__file__).parents[2] / "lib" / "Mrm"
for filename, function in checks:
    text = (root / filename).read_text(encoding="utf-8")
    start = text.index(function)
    end = text.index("\n}\n", start)
    body = text[start:end]
    guard = body.index("if (res_id_return == NULL)")
    clear = body.index("*res_id_return = 0;", guard)
    next_failure = body.index("return MrmFAILURE", clear)
    if not guard < clear < next_failure:
        raise SystemExit(f"{function} does not initialize its output before failure")

print("Mrm direct RID allocators initialize failed outputs")
