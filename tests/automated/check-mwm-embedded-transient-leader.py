#!/usr/bin/env python3
"""Check panel transient handling tolerates panel startup and teardown."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmManage.c").read_text(
    encoding="utf-8"
)
start = source.index("Handle case of transients that derive from embedded clients")
end = source.index("if (IsEmbeddedClient", start)
body = source[start:end]

assert "!XFindContext" in body
assert "pCDfp != NULL" in body
assert body.index("pCDfp != NULL") < body.index("pCD->transientLeader = pCDfp;")
assert "pCD->transientLeader = pCDfp;" not in body[: body.index("!XFindContext")]
assert "pCD->pSD->wPanelist != NULL" in body
assert "O_Shell (pPanelist) != NULL" in body

win_info = (Path(__file__).parents[2] / "clients/mwm/WmWinInfo.c").read_text(
    encoding="utf-8"
)
geometry_start = win_info.rindex("AdjustSlideOutGeometry")
geometry_end = win_info.index("#endif  /* PANELIST */", geometry_start)
geometry = win_info[geometry_start:geometry_end]
assert "pPanelist == NULL || O_Shell(pPanelist) == NULL" in geometry
assert geometry.index("pPanelist == NULL || O_Shell(pPanelist) == NULL") < geometry.index("XtWindow(O_Shell(pPanelist))")

function = (Path(__file__).parents[2] / "clients/mwm/WmFunction.c").read_text(
    encoding="utf-8"
)
toggle_start = function.index("F_Toggle_Front_Panel")
toggle_end = function.index("F_Version", toggle_start)
toggle = function[toggle_start:toggle_end]
assert "if (pPanelist && O_Shell(pPanelist))" in toggle
version = function[function.index("F_Version", toggle_end):]
assert "if (pPanelist && O_Shell(pPanelist))" in version

print("PASS MWM handles unavailable front panels without losing transient state")
