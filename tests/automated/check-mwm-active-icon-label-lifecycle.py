from pathlib import Path

source = Path("clients/mwm/WmIDecor.c").read_text(encoding="utf-8")
hide_start = source.index("void HideActiveIconText")
hide_end = source.index("void MoveActiveIconText", hide_start)
hide = source[hide_start:hide_end]
move_start = source.index("void MoveActiveIconText")
move_end = source.index("END OF FUNCTION  MoveActiveIconText", move_start)
move = source[move_start:move_end]

assert "Window label_window = None;" in hide
assert "if (label_window != None)" in hide
assert "XDeleteContext (DISPLAY, label_window" in hide
assert "pcd != NULL && pcd->pSD != NULL" in move
assert "pcd->pSD->activeIconTextWin != None" in move
print("PASS MWM active icon-label lifecycle validates resources")
