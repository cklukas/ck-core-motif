#!/usr/bin/env python3
"""Check MWM identity selection accepts an empty argument vector."""
from pathlib import Path
source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmMain.c").read_text()
start = source.index("int WmReturnIdentity")
end = source.index("} /* END OF FUNCTION WmReturnIdentity */", start)
body = source[start:end]
assert "if (argc <= 0 || argv == NULL || argv[0] == NULL)" in body
assert body.index("argv[0] == NULL") < body.index("strlen(argv[0])")
print("PASS MWM identity handles empty argument vectors")
