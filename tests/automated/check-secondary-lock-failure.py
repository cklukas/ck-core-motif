#!/usr/bin/env python3
"""Check that a refused secondary transfer releases its synchronous lock."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("SecondaryConvertHandler(Widget w")
end = source.index("\n/*ARGSUSED*/\nstatic void\nReleaseSecondaryLock", start)
region = source[start:end]
failure = region[region.index("_XmDestinationHandler"):]
assert "ReleaseSecondaryLock(w, XmOTHER, NULL);" in failure
assert failure.index("ReleaseSecondaryLock(w, XmOTHER, NULL);") < failure.index("cs -> status = XmCONVERT_REFUSE")
print("refused secondary transfer releases the synchronous lock")
