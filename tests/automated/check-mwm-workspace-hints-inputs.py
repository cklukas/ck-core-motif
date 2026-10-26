#!/usr/bin/env python3
"""Check that MWM workspace-hints output parameters are validated."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmProperty.c").read_text(
    encoding="utf-8"
)
start = source.index("Status GetWorkspaceHints")
end = source.index("\n} /* END OF FUNCTION GetWorkspaceHints */", start)
region = source[start:end]

assert "ppWsAtoms == NULL || pCount == NULL || pbAll == NULL" in region
assert "return (BadValue);" in region
assert "rcode == Success && pWsHints == NULL" in region
assert "return (BadAlloc);" in region
assert "pWsHints->numWorkspaces > 0 && pWsHints->pWorkspaces == NULL" in region
assert "return (BadValue);" in region
assert region.index("ppWsAtoms == NULL") < region.index("*ppWsAtoms = NULL;")
print("PASS MWM workspace-hints output parameters are validated")
