from pathlib import Path

source = Path("clients/mwm/WmFunction.c").read_text(encoding="utf-8")
start = source.index("Boolean F_Raise")
end = source.index("END OF FUNCTION F_Raise", start)
body = source[start:end]

assert "size_t slen, len, index;" in body
assert "slen = strlen(args);" in body
assert "slen = strlen(args) - 2" not in body
assert "if (len == 0)" in body
print("PASS MWM raise argument traversal avoids unsigned length underflow")
