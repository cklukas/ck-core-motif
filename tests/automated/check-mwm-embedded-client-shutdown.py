from pathlib import Path

source = Path("clients/mwm/WmManage.c").read_text(encoding="utf-8")
start = source.index("UnParentControls(")
end = source.index("END OF FUNCTION UnParentControl", start)
body = source[start:end]

assert "pCD != NULL && !(pCD->clientFlags & CLIENT_DESTROYED)" in body
assert body.index("CLIENT_DESTROYED") < body.index("XReparentWindow")
print("PASS MWM embedded shutdown skips destroyed clients")
