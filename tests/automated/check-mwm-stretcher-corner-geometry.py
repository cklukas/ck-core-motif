#!/usr/bin/env python3
"""Check compact-dimension protection in MWM stretcher corners."""

from pathlib import Path


source = Path("clients/mwm/WmGraphics.c").read_text(encoding="utf-8")
start = source.index("void StretcherCorner (RList *prTop")
end = source.index("} /* END OF FUNCTION StretcherCorner", start)
body = source[start:end]

assert "prTop == NULL || prBot == NULL" in body
assert "swidth < 2" in body
assert "cwidth < 3 || cheight < 3" in body
assert "swidth > cwidth || swidth > cheight" in body
guard = body.index("if (prTop == NULL")
assert guard < body.index("cwidth-2")
assert guard < body.index("cheight-3")

print("PASS MWM stretcher-corner geometry rejects undersized inputs")
