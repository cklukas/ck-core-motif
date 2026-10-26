#!/usr/bin/env python3
"""Check DataField malformed transfer metadata releases incoming values."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
start = source.rindex("df_DropTransferCallback(")
end = source.find("\nstatic ", start)
body = source[start:] if end == -1 else source[start:end]
assert "type == NULL" in body
assert "length == NULL" in body
assert "if (value != NULL)" in body
assert "XtFree((char *)value)" in body
assert "XmNtransferStatus, XmTRANSFER_FAILURE" in body

print("DataField malformed transfer metadata releases incoming values")
