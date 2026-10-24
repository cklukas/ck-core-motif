#!/usr/bin/env python3
"""Check Mrm pointer-list helpers validate null inputs before dereferencing."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmptrlist.c"
text = source.read_text(encoding="utf-8")
checks = {
    "UrmPlistInit": "list_id_return == NULL",
    "UrmPlistResize": "list_id == NULL",
    "UrmPlistAppendPointer": "list_id == NULL",
    "UrmPlistAppendString": "list_id == NULL || stg == NULL",
    "UrmPlistFindString": "list_id == NULL || stg == NULL",
}
for name, guard in checks.items():
    start = text.index(name + " (", text.index(name))
    end = text.index("\n}\n", start)
    region = text[start:end]
    guard_pos = region.index(guard)
    body = region[region.index("{") + 1:]
    body_guard_pos = body.index(guard)
    if name == "UrmPlistFindString":
        deref = body.index("UrmPlistNum(list_id)")
        if body_guard_pos > deref:
            raise SystemExit(f"{name} checks after list dereference")
    elif name == "UrmPlistInit":
        deref = body.index("*list_id_return")
        if body_guard_pos > deref:
            raise SystemExit(f"{name} checks after output dereference")
    else:
        if "return" not in body[body_guard_pos:body_guard_pos + 180]:
            raise SystemExit(f"{name} has no early null-input return")

print("Mrm pointer-list helpers validate null inputs")
