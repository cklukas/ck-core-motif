#!/usr/bin/env python3
"""Check that MWM invoke-command arguments are fully range checked."""
from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "clients" / "mwm" / "WmFunction.c").read_text()
start = source.index("parse_invoke_unsigned")
end = source.index("\n}\n\n\n\f", start)
helper = source[start:end]
invoke_start = source.index("Boolean F_InvokeCommand")
invoke = source[invoke_start:source.index("\n} /* END OF FUNCTION F_InvokeCommand", invoke_start)]

assert "parsed = strtoul(start, &end, 10);" in helper
assert "errno == ERANGE" in helper
assert "parsed > maximum" in helper
assert "sscanf(args" not in invoke
assert "*cursor != '\\0'" in invoke
assert "UINT_MAX" in invoke and "ULONG_MAX" in invoke
print("MWM invoke-command input contract passed")
