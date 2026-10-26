from pathlib import Path

source = Path("clients/mwm/WmCDecor.c").read_text(encoding="utf-8")
for name, drawable in (("FrameExposureProc", "clientFrameWin"),
                       ("BaseWinExposureProc", "clientBaseWin")):
    start = source.index(f"void {name}")
    end = source.index("END OF FUNCTION", start)
    body = source[start:end]
    assert f"pcd == NULL || pcd->{drawable} == None" in body
    assert body.index(f"{drawable} == None") < body.index("XFillRectangles")
print("PASS MWM frame exposure validates drawables")
