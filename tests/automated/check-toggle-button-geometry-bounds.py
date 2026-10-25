#!/usr/bin/env python3
"""Check that ToggleButton drawing paths handle undersized widgets safely."""
from pathlib import Path

source = Path(__file__).resolve().parents[2] / "lib" / "Xm" / "ToggleB.c"
text = source.read_text(encoding="utf-8")


def function_body(name, next_name):
    start = text.index("static void \n" + name)
    end = text.index("static void \n" + next_name, start)
    return text[start:end]


shadow = function_body("DrawToggleShadow", "DrawToggleLabel")
label = function_body("DrawToggleLabel", "DrawEtchedInMenu")
etched_start = text.index("static void \nDrawEtchedInMenu")
etched = text[etched_start:text.index("\f", etched_start + 20)]

required = [
    (shadow, "top_gc == NULL || bot_gc == NULL"),
    (shadow, "width = (int)tb->core.width - (hilite_thickness << 1);"),
    (label, "(int)tb->core.width <= 2 * (int)margin"),
    (label, "fw = (int)tb->core.width - 2 * (int)margin;"),
    (label, "if (fw <= 0 || fh <= 0)"),
    (etched, "(int)tb->core.height <= 2 * (int)margin"),
]
missing = [fragment for body, fragment in required if fragment not in body]
if missing:
    raise SystemExit("missing ToggleButton geometry guard: " + ", ".join(missing))

for name, body in (
    ("DrawToggleLabel", label),
    ("DrawEtchedInMenu", etched),
):
    if "tb->core.width - 2 * margin" in body or "tb->core.height - 2 * margin" in body:
        raise SystemExit(f"{name} still performs unsigned geometry subtraction")

print("ToggleButton drawing paths guard undersized geometry and missing shadow GCs")
