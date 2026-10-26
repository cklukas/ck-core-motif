#!/usr/bin/env python3
"""Check DataField drops reject a missing export-target array."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
start = source.rindex("df_HandleDrop(\n        Widget w")
end = source.find("\nstatic ", start)
body = source[start:] if end == -1 else source[start:end]
assert "numExportTargets != 0 && exportTargets == NULL" in body
assert "XmNtransferStatus, XmTRANSFER_FAILURE" in body
assert "XmDropTransferStart(drag_cont, args, n)" in body

print("DataField drops reject a missing export-target array")
