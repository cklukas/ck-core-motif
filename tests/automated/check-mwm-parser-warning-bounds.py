#!/usr/bin/env python3
"""Check that WSM parser warnings are dynamically formatted."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text()
start = source.index("void\nPWarning")
end = source.index("} /* END OF FUNCTION PWarning */", start)
region = source[start:end]
assert "char pch[MAXWMPATH+1]" not in region
assert "sprintf (pch" not in region
assert "snprintf(NULL, 0, pWarningStringFile" in region
assert "(unsigned int)messageLength < UINT_MAX" in region
assert "XtMalloc((Cardinal)messageLength + 1)" in region
assert "XtMalloc((unsigned int)messageLength + 1)" not in region
assert "XtFree(formattedMessage);" in region
assert "_DtSimpleError (wmGD.mwmName" in region
print("mwm parser warnings are dynamically bounded")
