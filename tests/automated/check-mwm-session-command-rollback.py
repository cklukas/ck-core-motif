#!/usr/bin/env python3
"""Check MWM session-command parsing publishes only complete argv arrays."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text(
    encoding="utf-8"
)
start = source.index("void ParseSessionCommand")
body = source[start : source.index("} /* END OF FUNCTION ParseSessionCommand", start)]
assert "pSD->pDtSessionItems[count].commandArgv = NULL;" in body
assert "pSD->pDtSessionItems[count].commandArgc = 0;" in body
assert "string_length > UINT_MAX - 1" in body
assert "XtFree((char *)argv[cleanup_index]);" in body
assert body.count("XtFree((char *)argv[cleanup_index]);") >= 3
assert "return;" in body[body.index("Insufficient memory for commandArgv item") :]

print("MWM session-command parsing rolls back incomplete argv arrays")
