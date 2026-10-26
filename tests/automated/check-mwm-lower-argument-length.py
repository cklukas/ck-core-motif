from pathlib import Path

source = Path("clients/mwm/WmFunction.c").read_text(encoding="utf-8")
start = source.index("Boolean F_Lower")
end = source.index("END OF FUNCTION F_Lower", start)
body = source[start:end]

assert "size_t slen, len, index;" in body
assert "slen = strlen(args);" in body
assert "slen = strlen(args) - 2" not in body
assert "if (len == 0)" in body
print("PASS MWM lower argument traversal avoids unsigned length underflow")
