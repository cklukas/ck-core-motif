#!/usr/bin/env python3
"""Check that UIL real literals reject non-finite and malformed input."""
from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "clients" / "uil" / "UilLexAna.c").read_text()
start = source.index("case token_real:")
end = source.index("\n    case token_ff:", start)
region = source[start:end]

assert "d_real = strtod(literal, &end);" in region
assert "end == literal || *end != '\\0'" in region
assert "errno == ERANGE" in region
assert "!isfinite(d_real)" in region
assert "d_real = 0.0;" in region
assert "atof(" not in region
print("UIL real-literal input contract passed")
