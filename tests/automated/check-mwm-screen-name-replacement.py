#!/usr/bin/env python3
"""Check MWM screen-name and workspace-name growth remains failure-atomic."""
from pathlib import Path

root = Path(__file__).parents[2]
init = (root / "clients/mwm/WmInitWs.c").read_text(encoding="utf-8")
resource = (root / "clients/mwm/WmResource.c").read_text(encoding="utf-8")

init_start = init.index("void\nInitWmScreen")
init_end = init.index("\n} /* END OF FUNCTION  InitWmScreen */", init_start)
init_region = init[init_start:init_end]
assert "unsigned char *resized;" in init_region
assert "resized = (unsigned char *)XtRealloc (wmGD.screenNames[sNum]," in init_region
assert "if (resized == NULL)" in init_region
assert "wmGD.screenNames[sNum] = resized;" in init_region
assert "if ((wmGD.screenNames[sNum] =" not in init_region

ws_start = resource.index("void \nProcessWorkspaceList")
ws_end = resource.index("\n} /* END OF FUNCTION ProcessWorkspaceList */", ws_start)
ws_region = resource[ws_start:ws_end]
assert "char **resizedNames;" in ws_region
assert "if (wsNameCount == wsNamesAlloced)" in ws_region
assert "resizedNames = (char **)XtRealloc(" in ws_region
assert "if (resizedNames == NULL)" in ws_region
assert "ppchWsNames = resizedNames;" in ws_region
assert "if (!(ppchWsNames = (char **) XtRealloc" not in ws_region
assert "XtFree((char *)ppchWsNames);" in ws_region
print("PASS MWM screen and workspace name growth is failure-atomic")
