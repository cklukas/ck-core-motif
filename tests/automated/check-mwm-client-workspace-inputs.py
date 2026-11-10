#!/usr/bin/env python3
"""Check MWM session workspace aggregation guards its client input."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmXSMP.c").read_text(
    encoding="utf-8"
)
start = source.index("getClientWorkspaces(ClientData *pCD)")
end = source.index("\n}\n\n#endif /* WSM */", start)
body = source[start:end]

assert "WmScreenData *pSD;" in body
assert "WmScreenData *pSD = pCD->pSD;" not in body
guard = body.index("if (pCD == NULL)")
assignment = body.index("pSD = pCD->pSD;")
assert guard < assignment
assert body.index("if (pSD == NULL", assignment) > assignment
print("PASS MWM client workspace aggregation guards null client data")
