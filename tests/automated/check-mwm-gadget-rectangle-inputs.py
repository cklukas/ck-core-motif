#!/usr/bin/env python3
"""Check null-client handling in MWM frame-gadget helpers."""

from pathlib import Path


source = Path("clients/mwm/WmCDecor.c").read_text(encoding="utf-8")


def body(marker: str, end_marker: str) -> str:
    start = source.index(marker)
    return source[start : source.index(end_marker, start)]


allocate = body("Boolean AllocateGadgetRectangles (ClientData *pcd)",
                "} /* END OF FUNCTION  AllocateGadgetRectangles  */")
compute = body("void ComputeGadgetRectangles (ClientData *pcd)",
               "} /* END OF FUNCTION  ComputeGadgetRectangles   */")

assert "if (pcd == NULL)" in allocate
assert "return (FALSE);" in allocate
assert allocate.index("if (pcd == NULL)") < allocate.index("pcd->decor")
assert "if (pcd == NULL)" in compute
assert "return;" in compute
assert compute.index("if (pcd == NULL)") < compute.index("pcd->decor")

print("PASS MWM gadget-rectangle helpers reject null clients")
