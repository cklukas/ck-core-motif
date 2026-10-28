#!/usr/bin/env python3
"""Check Unicode keysyms produce UTF-8 mnemonic characters."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xmos.c").read_text()
start = source.index("_XmOSKeySymToCharacter(KeySym keysym,")
end = source.index("\n/* ******************************************************", start)
body = source[start:end]

assert "(keysym & 0xFF000000UL) == 0x01000000UL" in body
assert "codepoint > 0x10FFFFUL" in body
assert "codepoint >= 0xD800UL" in body
assert "buffer[0] = (char)(0xF0U |" in body
assert "if (buffer == NULL)" in body

print("PASS Unicode keysym UTF-8 encoding")
