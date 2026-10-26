from pathlib import Path

source = Path("clients/mwm/WmFunction.c").read_text(encoding="utf-8")
start = source.index("Boolean F_InvokeCommand")
end = source.index("END OF FUNCTION F_InvokeCommand", start)
body = source[start:end]

assert "CARD32 invokingWindow = 0;" in body
assert "(unsigned long)pCD->client > (unsigned long)UINT_MAX" in body
assert "invokingWindow = (CARD32)pCD->client;" in body
assert "SendInvokeMessage(commandID" in body
assert "invokingWindow," in body
print("PASS MWM invoke-command window conversion is range checked")
