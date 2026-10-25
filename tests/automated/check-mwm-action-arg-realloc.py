#!/usr/bin/env python3
"""Check MWM action-argument reallocations preserve ownership on failure."""

from pathlib import Path


root = Path(__file__).parents[2]
parser = (root / "clients" / "mwm" / "WmResParse.c").read_text(encoding="utf-8")
start = parser.rindex("Boolean ParseWmFuncActionArg")
body = parser[start : parser.index("#endif /* PANELIST */", start)]
assert "char *new_pch;" in body
assert "DtActionArg *new_aap;" in body
assert "if (new_pch == NULL)" in body
assert "if (new_aap == NULL)" in body
assert "pAP->aap = new_aap;" in body

event = (root / "clients" / "mwm" / "WmEvent.c").read_text(encoding="utf-8")
cleanup = event[event.index("else if ((functionTable[iFuncIndex].parseProc ==") : event.index("\n\t\t\t}\n", event.index("else if ((functionTable[iFuncIndex].parseProc =="))]
assert "XtFree ((char *) pAP->aap);" in cleanup

print("MWM action-argument reallocations and cleanup are guarded")
