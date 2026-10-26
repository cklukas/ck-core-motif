#!/usr/bin/env python3
"""Check failure-safe reusable bevel-list allocation in DepressGadget."""

from pathlib import Path


source = Path("clients/mwm/WmCDecor.c").read_text(encoding="utf-8")
start = source.index("Boolean DepressGadget (ClientData *pcd, int gadget, Boolean depressed)")
end = source.index("} /* END OF FUNCTION  DepressGadget", start)
body = source[start:end]

assert "if (pTopRect == NULL)" in body
assert "pTopRect = AllocateRList(2);" in body
assert "if (pBotRect == NULL)" in body
assert "pBotRect = AllocateRList(2);" in body
assert "if (pTopRect == NULL || pBotRect == NULL)" in body
assert "return(FALSE);" in body
assert body.index("return(FALSE);") < body.index("pTopRect->used")

print("PASS MWM gadget bevel allocation failure is safe")
