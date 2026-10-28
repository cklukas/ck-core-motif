#!/usr/bin/env python3
"""Check Stepper labels cannot wrap through Motif's Position type."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Stepper.c").read_text(
    encoding="utf-8"
)
assert "static Position bounded_position(long long value)" in source
assert "char text[32]; int split;" in source
assert "bounded_position(((long long)s->core.height - text_height) / 2)" in source
assert "bounded_position((long long)centers[i] - symbol_width / 2)" in source
assert "bounded_position((long long)i * cell_height" in source
print("PASS Stepper label coordinates stay bounded")
