from pathlib import Path

source = Path("clients/mwm/WmCDecor.c").read_text(encoding="utf-8")
for name in ("ShowActiveClientFrame", "ShowInactiveClientFrame"):
    start = source.index(name + " (ClientData")
    end = source.index("END OF FUNCTION", start)
    body = source[start:end]
    assert "pcd == NULL || pcd->clientFrameWin == None" in body
    assert "pcd->clientTitleWin != None" in body
print("PASS MWM client-frame state validates hierarchy")
