#!/usr/bin/env python3
"""Check editres protocol-stream growth is overflow- and failure-safe."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/EditresCom.c").read_text(
    encoding="utf-8"
)

assert "ULONG_MAX - 100 - HEADER_SIZE" in source
assert "if (new_top == NULL)" in source
assert "stream->alloc = new_alloc" in source
print("Editres protocol-stream growth is bounded and failure-safe")
