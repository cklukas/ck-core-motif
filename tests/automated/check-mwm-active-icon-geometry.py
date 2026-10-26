from pathlib import Path

source = Path("clients/mwm/WmIDecor.c").read_text(encoding="utf-8")
show_start = source.index("void ShowActiveIconText")
show_end = source.index("END OF FUNCTION ShowActiveIconText", show_start)
move_start = source.index("void MoveActiveIconText")
move_end = source.index("END OF FUNCTION  MoveActiveIconText", move_start)

assert "if (!XGetGeometry" in source[show_start:show_end]
assert "if (!XGetGeometry" in source[move_start:move_end]
print("PASS MWM active icon geometry queries check status")
