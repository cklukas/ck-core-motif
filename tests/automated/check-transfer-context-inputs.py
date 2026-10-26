#!/usr/bin/env python3
"""Check transfer-context APIs reject null callback handles."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text(encoding="utf-8")
for function, marker in (
    ("XmTransferDone", "\nXmTransferDone("),
    ("XmTransferSetParameters", "\nXmTransferSetParameters("),
    ("XmTransferValue", "\nXmTransferValue("),
    ("XmTransferStartRequest", "\nXmTransferStartRequest("),
    ("XmTransferSendRequest", "\nXmTransferSendRequest("),
):
    start = source.index(marker)
    end = source.find("\n}", start)
    assert "if (tc == NULL)\n    return;" in source[start:end], function
print("PASS transfer-context APIs validate null handles")
