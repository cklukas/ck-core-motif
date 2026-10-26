#!/usr/bin/env python3
"""Check MWM all-client query conversion validates inputs and lengths."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmProtocol.c").read_text(
    encoding="utf-8"
)
start = source.index("wmq_convert_all_clients (")
end = source.index("} /* END OF FUNCTION wmq_convert_all_clients */", start)
region = source[start:end]
assert "screen < 0 || screen >= wmGD.numScreens" in region
assert "pType_return == NULL || pValue_return == NULL" in region
assert "pLength_return == NULL || pFormat_return == NULL" in region
assert "pEntry->type != MINIMIZED_STATE && pEntry->pCD != NULL" in region
assert "*pLength_return = (unsigned long)curXids;" in region
assert "*pLength_return = curXids;" not in region
print("PASS MWM all-client query conversion validates inputs")
