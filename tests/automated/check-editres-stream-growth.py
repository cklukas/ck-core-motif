#!/usr/bin/env python3
"""Check editres reply-stream growth is overflow- and failure-safe."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "EditresCom.c").read_text(
    encoding="utf-8"
)
start = source.index('\nReplyReserve(')
end = source.index('\nReplyPut8(', start)
region = source[start:end]

assert "EDITRES_MAX_REPLY - extra" in region
assert "new_buf = (unsigned char *) realloc(stream->buf, HEADER_SIZE + new_alloc);" in region
assert "if (new_buf == NULL)" in region
assert "stream->failed = TRUE;" in region
assert "stream->alloc = new_alloc;" in region
print("Editres protocol-stream growth is bounded and failure-safe")
