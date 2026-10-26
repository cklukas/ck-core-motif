from pathlib import Path

source = Path("clients/mwm/WmIDecor.c").read_text(encoding="utf-8")
start = source.index("Boolean MakeIcon")
end = source.index("END OF FUNCTION MakeIcon", start)
body = source[start:end]

assert "if (pWsc->iconFrameWin == None)" in body
assert "if (pcd->iconFrameWin == None)" in body
assert "return(FALSE);" in body
print("PASS MWM icon-frame allocation is guarded")
