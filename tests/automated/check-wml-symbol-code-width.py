#!/usr/bin/env python3
"""Check WML resolver symbol codes retain the full generated code range."""

from pathlib import Path


source = (Path(__file__).parents[2] / "tools" / "wml" / "wml.h").read_text(
    encoding="utf-8"
)

assert "short int\t\tsym_code" not in source
assert source.count("int\t\t\tsym_code;") == 6
assert "int\t\t\trelated_code;" in source

print("PASS WML resolver symbol codes use full-width integers")
