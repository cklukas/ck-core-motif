from pathlib import Path

source = Path("clients/mwm/WmFunction.c").read_text(encoding="utf-8")
for function in ("F_Refresh (", "F_Refresh_Win ("):
    start = source.index(function)
    body = source[start:source.index("END OF FUNCTION", start)]
    assert "win = XCreateWindow" in body
    assert "if (win == None)" in body
    assert body.index("win == None") < body.index("XMapWindow")

print("PASS MWM refresh commands validate temporary window creation")
