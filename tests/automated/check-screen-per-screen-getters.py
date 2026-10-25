#!/usr/bin/env python3
"""Check that Screen per-screen getters validate screen/widget inputs."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Screen.c"
text = source.read_text(encoding="utf-8")
for needle in (
    "return(xmScreen->screen.color_calc_proc);",
    "return(xmScreen->screen.color_alloc_proc);",
    "return XmMATCH_DEPTH;",
    "if (w == NULL)\n        return None;",
):
    if needle not in text:
        raise SystemExit(f"per-screen getter guard is missing: {needle}")

print("per-screen getter guards present")
