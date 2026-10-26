from pathlib import Path

source = Path("clients/mwm/WmFunction.c").read_text(encoding="utf-8")
start = source.index("Boolean F_InvokeCommand")
end = source.index("END OF FUNCTION F_InvokeCommand", start)
body = source[start:end]

assert "unsigned int parsedCommandID, parsedClientWindow;" in body
assert "unsigned long parsedSelection;" in body
assert 'sscanf(args, "%u %u %lu", &parsedCommandID, &parsedClientWindow,' in body
assert "(int *)&commandID" not in body
assert "commandID = (CARD32)parsedCommandID;" in body
assert "notifySelection = (Atom)parsedSelection;" in body
print("PASS MWM invoke-command parsing uses matching unsigned types")
