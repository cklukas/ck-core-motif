#!/usr/bin/env python3
"""Check that StringToEntity validates Xt converter inputs."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c").read_text(
    encoding="utf-8"
)
start = text.index("StringToEntity(\n        Display *disp")
start = text.index("{", start)
body = text[start : start + 800]
for needle in ("args == NULL", "n_args == NULL", "from == NULL", "to == NULL",
               "widget == NULL"):
    if needle not in body:
        raise SystemExit(f"StringToEntity guard is missing: {needle}")

print("StringToEntity input guards present")
