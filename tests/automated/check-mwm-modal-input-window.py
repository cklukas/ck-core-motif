from pathlib import Path

source = Path("clients/mwm/WmWinList.c").read_text(encoding="utf-8")
setup = source[source.index("void SetupSystemModalState"):
               source.index("END OF FUNCTION SetupSystemModalState")]
undo = source[source.index("void UndoSystemModalState"):
              source.index("END OF FUNCTION UndoSystemModalState")]

assert "if (ACTIVE_PSD->inputScreenWindow != None)" in setup
assert "if (pSD->inputScreenWindow != None)" in source
assert "if (wmGD.Screens[scr].inputScreenWindow != None)" in undo
print("PASS MWM modal input window operations validate window IDs")
