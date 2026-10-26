#!/usr/bin/env python3
"""Ensure private Text encoding helpers reject invalid input before use."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c").read_text(
    encoding="utf-8"
)

bytes_to_chars = source[source.index("_XmTextBytesToCharacters("):]
bytes_to_chars = bytes_to_chars[:bytes_to_chars.index("/* Convert an array")]
assert "num_chars <= 0" in bytes_to_chars
assert "characters == NULL" in bytes_to_chars
assert "bytes == NULL" in bytes_to_chars
assert "max_char_size <= 0" in bytes_to_chars

chars_to_bytes = source[source.index("_XmTextCharactersToBytes("):]
chars_to_bytes = chars_to_bytes[:chars_to_bytes.index("static XmTextPosition")]
assert "num_chars <= 0" in chars_to_bytes
assert "bytes == NULL || characters == NULL" in chars_to_bytes
assert "max_char_size <= 0" in chars_to_bytes

print("Text conversion helpers reject invalid buffers, lengths, and widths")
