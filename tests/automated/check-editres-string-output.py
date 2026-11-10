#!/usr/bin/env python3
"""Check Editres string output preserves the wire length."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "EditresCom.c").read_text(
    encoding="utf-8"
)
start = source.index('\nReplyPutString8(')
end = source.index('\n/*\tFunction Name: ReplyPutPath', start)
region = source[start:end]

assert "size_t len = (str != NULL) ? strlen(str) : 0;" in region
assert "if (len > EDITRES_MAX_CARD16)" in region
assert "len = EDITRES_MAX_CARD16;" in region
assert "ReplyPut16(stream, (unsigned int)len);" in region
assert "memcpy(stream->buf + HEADER_SIZE + stream->size, str, len);" in region
print("PASS Editres string output preserves wire length")
