from pathlib import Path

source = Path("clients/mwm/WmIDecor.c").read_text(encoding="utf-8")
start = source.index("void ReparentIconWindow")
end = source.index("END OF FUNCTION ReparentIconWindow", start)
body = source[start:end]

assert "pcd == NULL || pcd->iconWindow == None" in body
assert "ICON_FRAME_WIN(pcd) == None" in body
assert "if (!XGetGeometry" in body
assert body.index("if (!XGetGeometry") < body.index("XConfigureWindow")
assert body.index("if (!XGetGeometry") < body.index("XReparentWindow")
print("PASS MWM client icon reparenting validates external windows")
