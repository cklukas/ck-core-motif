#!/usr/bin/env python3
"""Check MWM session persistence rejects partial manager/client state."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmXSMP.c").read_text(
    encoding="utf-8"
)


def body(start: str, end: str) -> str:
    begin = source.index(start)
    return source[begin : source.index(end, begin)]


xsmp = body("static Boolean\nsaveXSMPClient", "\n/*\n *  Assumes: pCD->smClientID is NULL")
assert "fp == NULL || pCD == NULL || pCD->pSD == NULL" in xsmp
assert "pCD->smClientID == NULL" in xsmp
assert "if (pCD->pWsList != NULL)" in xsmp

proxy = body("static Boolean\nsaveProxyClient", "\nstatic void\ndbRemoveProxyClientEntry")
assert "fp == NULL || pCD == NULL || pCD->pSD == NULL" in proxy
assert "if (pCD->pWsList != NULL)" in proxy

callbacks = body("void\nAddSMCallbacks", "\n/*\n *  Resign from session management")
assert callbacks.index("if (wmGD.topLevelW == NULL)") < callbacks.index("XtAddCallback")

save = body("XrmDatabase\nSaveClientResourceDB", "\n/*\n *  As with FindDtSessionMatch")
assert "wmGD.topLevelW == NULL || wmGD.numScreens < 0" in save
assert "wmGD.numScreens > 0 && wmGD.Screens == NULL" in save
assert save.index("if (pCD == NULL)") < save.index("pCD->smClientID")
assert "if (fclose(fp) != 0)" in save

match = body("Boolean\nFindClientDBMatch", "\nBoolean\nGetSmClientIdClientList")
assert match.index("if (workSpaceNamesP != NULL)") < match.index("if (pCD == NULL)")
assert "*workSpaceNamesP = NULL;" in match

listing = body("Boolean\nGetSmClientIdClientList", "\nvoid\nSortClientListByWorkspace")
assert "wmGD.numScreens < 0" in listing
assert "wmGD.numScreens > 0 && wmGD.Screens == NULL" in listing

print("PASS MWM session persistence rejects partial manager and client state")
