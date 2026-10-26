#!/usr/bin/env python3
"""Ensure malformed Motif DnD message subtypes are rejected safely."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragICC.c").read_text()

mapper_start = source.index("_XmMessageTypeToReason(")
mapper_end = source.index("\n}\n", mapper_start) + 3
mapper = source[mapper_start:mapper_end]
assert "if (messageType >= XtNumber(messageTable))" in mapper
assert "return _XmNUMBER_DND_CB_REASONS;" in mapper

decoder_start = source.index("GetMessageData(")
decoder_end = source.index("/************************************************************************\n *\n *  SwapMessageData", decoder_start)
decoder = source[decoder_start:decoder_end]
assert "message_type >= XtNumber(messageTable)" in decoder
assert "return (XmICCEventType)-1;" in decoder

print("PASS Motif DnD message subtype bounds are checked")
