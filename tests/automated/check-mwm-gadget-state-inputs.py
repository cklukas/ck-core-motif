#!/usr/bin/env python3
"""Check null-client handling in MWM gadget-state helpers."""

from pathlib import Path


source = Path("clients/mwm/WmCDecor.c").read_text(encoding="utf-8")


def body(marker: str, next_marker: str) -> str:
    start = source.index(marker)
    return source[start : source.index(next_marker, start)]


push = body("void PushGadgetIn (ClientData *pcd, int gadget)", "void PopGadgetOut")
pop = body("void PopGadgetOut (ClientData *pcd, int gadget)", "#ifndef NO_SHAPE")
for function in (push, pop):
    assert "if (pcd == NULL)" in function
    assert "return;" in function
    assert function.index("if (pcd == NULL)") < function.index("pcd->decorFlags")

print("PASS MWM gadget-state helpers reject null clients")
