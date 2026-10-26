#!/usr/bin/env python3
"""Check that transfer parameter formats stay within X selection formats."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("XmTransferSetParameters(XtPointer transfer_id")
end = source.index("/************************************************************************/\n/* XmTransferValue", start)
region = source[start:end]
assert "parm_fmt != 0 && parm_fmt != 8 && parm_fmt != 16 && parm_fmt != 32" in region
assert region.index("parm_fmt != 0") < region.index("_XmWidgetToAppContext")
assert "if (parm_fmt == 0) parm_fmt = 8;" in region
print("transfer parameter formats are restricted to 8, 16, or 32 bits")
