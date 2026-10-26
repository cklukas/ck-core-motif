from pathlib import Path

source = Path("clients/mwm/WmCDecor.c").read_text(encoding="utf-8")
start = source.index("void DrawWindowTitle")
end = source.index("END OF FUNCTION DrawWindowTitle", start)
body = source[start:end]

assert "if (win == None)" in body
assert body.index("win == None") < body.index("XClearArea")
assert body.index("win == None") < body.index("WmDrawXmString")
print("PASS MWM window-title drawing validates drawable")
