from pathlib import Path

source = Path("clients/mwm/WmIDecor.c").read_text(encoding="utf-8")
start = source.index("void CreateActiveIconTextWindow")
end = source.index("END OF FUNCTION CreateActiveIconTextWindow", start)
body = source[start:end]

assert "if (pSD->activeIconTextWin == None)" in body
assert "pSD->activeLabelParent = None;" in body
assert "return;" in body
print("PASS MWM active icon-label allocation is guarded")
