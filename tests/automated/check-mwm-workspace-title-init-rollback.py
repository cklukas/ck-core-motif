#!/usr/bin/env python3
"""Check MWM workspace-title initialization is transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResource.c").read_text()
start = source.index("ProcessWorkspaceResources (WmWorkspaceData *pWS)")
end = source.index("} /* END OF FUNCTION ProcessWorkspaceResources */", start)
region = source[start:end]

assert "XmString replacement;" in region
copy = "replacement = XmStringCopy(pWS->title);"
assert copy in region
assert "if (replacement != NULL)" in region
assert "pWS->title = replacement;" in region
assert region.index(copy) < region.index("pWS->title = replacement;")
print("MWM workspace titles preserve their value on allocation failure")
