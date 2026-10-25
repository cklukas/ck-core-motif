#!/usr/bin/env python3
"""Check that include-stack allocation warnings avoid wmGD.tmpBuffer."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text()
start = source.index("static void\nWarnConfigAllocation")
end = source.index("\nstatic void ConfigStackInit", start)
helper = source[start:end]
assert "wmGD.tmpBuffer" not in helper
assert "snprintf(NULL, 0, format, path)" in helper
assert "(unsigned int)messageLength < UINT_MAX" in helper
assert "XtMalloc((Cardinal)messageLength + 1)" in helper
assert "XtMalloc((unsigned int)messageLength + 1)" not in helper
assert "XtFree(message);" in helper
init_start = source.index("static void ConfigStackInit (char *pchFileName)")
init_end = source.index("static FILE *\nConfigStackPush", init_start)
init = source[init_start:init_end]
push_start = source.index("static FILE *\nConfigStackPush", init_end)
push_end = source.index("\n", push_start)
push = source[push_start:push_end]
assert init.count("WarnConfigAllocation(pchFileName);") == 1
assert push.count("WarnConfigAllocation((char *)pchFileName);") == 1
assert "sprintf ((char *)wmGD.tmpBuffer" not in init + push
print("mwm include-stack allocation warnings are dynamically bounded")
