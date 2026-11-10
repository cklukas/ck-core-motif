#!/usr/bin/env python3
"""Check that f.action parser failures never retain a partial argument list."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text()
start = source.index("Boolean ParseWmFuncActionArg")
end = source.index("} /* END OF FUNCTION ParseWmFuncActionArg */", start)
body = source[start:end]

assert "Boolean parse_failed = False;" in body
assert "iArgSz = 0;" in body
assert "while ((allocated_string = GetString (linePP)) != NULL)" in body
assert "if (pAP->aap == NULL)" in body
assert "new_aap = (DtActionArg *)XtMalloc(" in body
assert "new_aap = (DtActionArg *)XtRealloc(" in body
assert body.count("parse_failed = True;") >= 5
cleanup = body.index("if (parse_failed)")
assert body.index("while (pAP->numArgs > 0)", cleanup) > cleanup
assert body.index("XtFree((char *)pAP->aap);", cleanup) > cleanup
assert body.index("XtFree((char *)pAP->actionName);", cleanup) > cleanup
assert body.index("*pArgs = NULL;", cleanup) > cleanup
assert body.index("return FALSE;", cleanup) > cleanup
print("MWM f.action parser rejects and cleans up partial argument lists")
