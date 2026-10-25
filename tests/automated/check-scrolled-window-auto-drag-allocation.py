#!/usr/bin/env python3
"""Guard ScrolledWindow auto-drag rectangle allocation failure paths."""

from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "ScrolledW.c").read_text(
    encoding="utf-8"
)
for function, output in (("GetHorRects", "hrect"), ("GetVertRects", "vrect")):
    start = text.index(function + "(", 20000)
    candidates = [p for p in (text.find("\n\nstatic void", start), text.find("\nstatic void", start)) if p != -1]
    end = min(candidates) if candidates else len(text)
    body = text[start:end]
    if f"*num_{output} = 0" not in body:
        raise SystemExit(f"{function} does not initialize an empty result")
    if f"if (*{output} == NULL)" not in body:
        raise SystemExit(f"{function} does not check allocation failure")
    if f"*num_{output} = 2" not in body:
        raise SystemExit(f"{function} does not publish successful count")

print("ScrolledWindow auto-drag allocation checks passed")
