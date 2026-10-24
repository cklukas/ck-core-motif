#!/usr/bin/env python3
"""Check that the localized screen-conflict warning is dynamically bounded."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmInitWs.c").read_text()
start = source.index('if ((wmGD.errorFlag)')
end = source.index('\n\t    else', start)
region = source[start:end]
assert 'char pch[80]' not in source
assert 'sprintf(pch' not in region
assert 'snprintf(NULL, 0, format, sNum)' in region
assert 'XtMalloc((unsigned int)messageLength + 1)' in region
assert 'XtFree(message);' in region
print("mwm screen-conflict warning is dynamically bounded")
