#!/usr/bin/env python3
"""Check IconGadget container trait callbacks are guarded."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconG.c").read_text()
assert "if (container_id == NULL)" in source
assert "container_trait != NULL && container_trait->getValues != NULL" in source
assert "container_id != NULL ?" in source
print("PASS IconGadget container trait callbacks are guarded")
