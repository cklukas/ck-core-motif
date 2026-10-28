#!/usr/bin/env python3
"""Check UIL resource-vector allocation sizes are validated before narrowing."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSarMod.c").read_text()
helper = source[source.index("static Cardinal\nsar_vector_bytes"):source.index("\n\f\n/*", source.index("static Cardinal\nsar_vector_bytes"))]
init = source[source.index("if ( uil_urm_variant == NULL )"):source.index("/*", source.index("if ( uil_urm_variant == NULL "))]

assert "if (maximum < 0)" in helper
assert "count > (size_t)UINT_MAX / sizeof(unsigned short int)" in helper
assert "return (Cardinal)(count * sizeof(unsigned short int));" in helper
for name in ("uil_max_object", "uil_max_arg", "uil_max_reason", "uil_max_child"):
    assert f"sar_vector_bytes({name})" in init
assert "sizeof(unsigned short int)*(uil_max_" not in init
print("UIL resource-vector allocation sizes are checked")
