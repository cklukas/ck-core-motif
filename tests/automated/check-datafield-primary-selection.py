#!/usr/bin/env python3
"""Check primary-selection request state allocation failure cleanup."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/DataF.c").read_text(
    encoding="utf-8"
)
start = source.rindex("prim_select = (_XmTextPrimSelect *)")
end = source.index("prim_select->position", start)
context = source[start:end]
required = (
    "if (prim_select == NULL)",
    "XtFree((char *)value);",
    "XtFree((char *)tmp_action->event);",
    "XtFree((char *)tmp_action);",
)
missing = [item for item in required if item not in context]
if missing:
    raise SystemExit("primary-selection allocation cleanup missing: " + ", ".join(missing))

print("DataField primary-selection checks passed")
