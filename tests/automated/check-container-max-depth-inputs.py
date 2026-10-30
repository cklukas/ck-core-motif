#!/usr/bin/env python3
"""Check optional Container max-depth traversal validates inputs."""
from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Container.c").read_text(
    encoding="utf-8"
)
start = source.rindex("static void FindMaxDepths")
region = source[start : source.index("#endif", start)]
assert "c == NULL || c->node_ptr == NULL || w == NULL" in region
assert "if (child == NULL)" in region
assert "if (cc == NULL)" in region

print("PASS optional Container max-depth traversal validates inputs")
