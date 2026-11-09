#!/usr/bin/env python3
"""Check malformed session switches cannot call Xlib or libc with NULL."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmResParse.c").read_text(
    encoding="utf-8"
)

state_start = source.index("ParseSessionClientState (WmScreenData *pSD")
state_end = source.index("} /* END OF FUNCTION ParseSessionClientState", state_start)
state = source[state_start:state_end]
assert "if (string == NULL)" in state
assert state.index("if (string == NULL)") < state.index("strcmp((char *)string")

geometry_start = source.index("ParseSessionGeometry (WmScreenData *pSD")
geometry_end = source.index("} /* END OF FUNCTION  ParseSessionGeometry", geometry_start)
geometry = source[geometry_start:geometry_end]
assert "if (string == NULL)" in geometry
assert geometry.index("if (string == NULL)") < geometry.index("XParseGeometry((char *)string")

for name in ("ParseSessionWorkspaces", "ParseSessionHost"):
    start = source.index(name + " (WmScreenData *pSD")
    end = source.index("} /* END OF FUNCTION " + name, start)
    body = source[start:end]
    assert "if (string == NULL)" in body
    assert body.index("if (string == NULL)") < body.index("CopySessionString(string)")

print("PASS MWM session parser rejects missing switch values")
