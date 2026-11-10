#!/usr/bin/env python3
"""Check MWM action-argument growth preserves ownership on failure."""

from pathlib import Path


root = Path(__file__).parents[2]
parser = (root / "clients" / "mwm" / "WmResParse.c").read_text(encoding="utf-8")
start = parser.rindex("Boolean ParseWmFuncActionArg")
body = parser[start : parser.index("#endif /* PANELIST */", start)]
assert "DtActionArg *new_aap;" in body
assert "size_t argument_length;" in body
assert "int new_arg_size;" in body
assert "unsigned char *allocated_string;" in body
assert "XtFree((char *)allocated_string);" in body
assert "pAP->actionName = NULL;" in body
assert "pAP->aap = NULL;" in body
assert "XtFree((char *)string);" in body
assert "argument_length > UINT_MAX - 1" in body
assert "if (new_aap == NULL)" in body
assert "iArgSz > INT_MAX - WM_ACTION_ARG_INCREMENT" in body
assert "SIZE_MAX / sizeof(DtActionArg)" in body
assert "pAP->aap = new_aap;" in body
assert "if (pAP->aap == NULL)" in body
assert "new_aap = (DtActionArg *)XtMalloc(" in body
assert "new_aap = (DtActionArg *)XtRealloc(" in body
assert "parse_failed = True;" in body
failure_cleanup = body.index("if (parse_failed)")
assert body.index("XtFree((char *)pAP->aap);", failure_cleanup) > failure_cleanup
assert body.index("XtFree((char *)pAP->actionName);", failure_cleanup) > failure_cleanup
assert body.index("*pArgs = NULL;", failure_cleanup) > failure_cleanup

event = (root / "clients" / "mwm" / "WmEvent.c").read_text(encoding="utf-8")
cleanup = event[event.index("else if ((functionTable[iFuncIndex].parseProc ==") : event.index("\n\t\t\t}\n", event.index("else if ((functionTable[iFuncIndex].parseProc =="))]
assert "XtFree ((char *) pAP->aap);" in cleanup

print("MWM action-argument growth and cleanup are guarded")
