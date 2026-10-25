#!/usr/bin/env python3
"""Check IconGadget validates container trait and parent callbacks."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconG.c").read_text()
assert "if (container_id == NULL)" in source
assert source.count("container_trait != NULL && container_trait->getValues != NULL") >= 2
assert "container_id != NULL ?" in source
