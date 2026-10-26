#!/usr/bin/env python3
"""Check that the destination transfer boundary validates its inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("_XmDestinationHandler(Widget wid")
end = source.index("\nstatic void\nFinishTransfer", start)
region = source[start:end]
assert "if (wid == NULL || selection == None)" in region
assert "return(False);" in region
assert region.index("return(False);") < region.index("XtDisplay(wid)")
print("destination handler validates widget and selection inputs")
