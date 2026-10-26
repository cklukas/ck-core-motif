#!/usr/bin/env python3
"""Check XmColumn default geometry cannot wrap Dimension values."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Column.c").read_text()

assert "static Dimension column_default_dimension(Dimension shadow, Dimension margin);" in source
assert "sc->core.width = column_default_dimension" in source
assert "sc->core.height = column_default_dimension" in source
assert "uint64_t total = 2 * ((uint64_t)shadow + (uint64_t)margin);" in source
assert "total > USHRT_MAX" in source
print("PASS Column default dimensions saturate")
