from pathlib import Path

source = Path("clients/mwm/WmFunction.c").read_text(encoding="utf-8")
start = source.index("Boolean F_Refresh_Win")
end = source.index("END OF FUNCTION F_Refresh_Win", start)
body = source[start:end]

assert "pCD->clientWidth <= 0 || pCD->clientHeight <= 0" in body
assert "pCD->maxWidth <= 0 || pCD->maxHeight <= 0" in body
assert body.index("clientWidth <= 0") < body.index("w = (unsigned int) pCD->clientWidth")
assert body.index("maxWidth <= 0") < body.index("w = (unsigned int) pCD->maxWidth")
print("PASS MWM refresh rejects non-positive signed geometry before casting")
