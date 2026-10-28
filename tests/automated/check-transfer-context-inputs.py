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
    body = source[start:end]
    assert "tc == NULL || tc->widget == NULL" in body, function
    assert "return;" in body, function
assert "status < XmTRANSFER_DONE_SUCCEED" in source
assert "status > XmTRANSFER_DONE_DEFAULT" in source
print("PASS transfer-context APIs validate null handles")
