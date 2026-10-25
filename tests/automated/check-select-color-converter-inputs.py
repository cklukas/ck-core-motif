#!/usr/bin/env python3
"""Check select-color conversion validates Xt converter inputs."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c").read_text(
    encoding="utf-8"
)
start = text.rfind("CvtStringToSelectColor(")
start = text.index("{", start)
body = text[start : start + 950]
for needle in ("disp == NULL", "num_args == NULL", "from_val == NULL",
               "to_val == NULL", "args == NULL"):
    if needle not in body:
        raise SystemExit(f"select-color converter guard is missing: {needle}")

print("select-color converter input guards present")
