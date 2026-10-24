#!/usr/bin/env python3
"""Check that configuration-reopen diagnostics are dynamically bounded."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text()
start = source.index("static void ConfigStackPop (void)\n{")
end = source.index("\n}\n\n", start)
region = source[start:end]
assert "char msg[MAXWMPATH+1]" not in region
assert "sprintf(msg" not in region
assert "snprintf(NULL, 0, format, fileName)" in region
assert "XtFree(message);" in region
print("mwm configuration-reopen warning is dynamically bounded")
