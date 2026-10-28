from pathlib import Path

source = Path("clients/mwm/WmFunction.c").read_text(encoding="utf-8")
start = source.index("Boolean F_InvokeCommand")
end = source.index("END OF FUNCTION F_InvokeCommand", start)
body = source[start:end]

assert "unsigned long parsedCommandID, parsedClientWindow, parsedSelection;" in body
assert body.count("parse_invoke_unsigned") >= 3
assert "while (isspace((unsigned char)*cursor))" in body
assert "*cursor != '\\0'" in body
assert "sscanf(" not in body
assert "(int *)&commandID" not in body
assert "commandID = (CARD32)parsedCommandID;" in body
assert "notifySelection = (Atom)parsedSelection;" in body
print("PASS MWM invoke-command parsing uses matching unsigned types")
