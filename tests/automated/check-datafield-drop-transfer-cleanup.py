#!/usr/bin/env python3
"""Check DataField transfer teardown frees incoming values and fails."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
start = source.rindex("df_DropTransferCallback(")
end = source.find("\nstatic ", start)
body = source[start:] if end == -1 else source[start:end]
assert "transfer_rec->widget == NULL" in body
assert "XmNtransferStatus, XmTRANSFER_FAILURE" in body
assert "if (value != NULL)" in body
assert "XtFree((char *)value)" in body

print("DataField transfer teardown frees incoming values and fails")
