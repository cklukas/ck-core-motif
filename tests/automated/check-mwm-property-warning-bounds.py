#!/usr/bin/env python3
"""Check that localized mwm property warnings are dynamically formatted."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWinInfo.c").read_text()
start = source.index("static XmString\nWmICCCMToXmString")
end = source.index("\n}\n\n", start)
region = source[start:end]
assert "char msg[200]" not in region
assert "sprintf(msg" not in region
assert region.count("WmWarningWithString") == 4
helper = source[source.index("static void\nWmWarningWithString"):start]
assert "snprintf(NULL, 0, format, value)" in helper
assert "XtMalloc((unsigned int)messageLength + 1)" in helper
print("mwm property warnings are dynamically bounded")
