#!/usr/bin/env python3
"""Check that the fatal font-load warning is not written to tmpBuffer."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResource.c").read_text()
start = source.index("if (! XmeRenderTableGetDefaultFont")
end = source.index("\n#ifndef NO_MULTIBYTE", start)
region = source[start:end]
assert "wmGD.tmpBuffer" not in region
assert "sprintf((char *)wmGD.tmpBuffer" not in region
assert "snprintf(NULL, 0, format, fontList)" in region
assert "XtMalloc((unsigned int)messageLength + 1)" in region
assert "XtFree(message);" in region
print("mwm font-load warning is dynamically bounded")
