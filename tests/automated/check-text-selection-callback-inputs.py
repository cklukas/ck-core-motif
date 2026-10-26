#!/usr/bin/env python3
"""Check Text and TextField selection callbacks validate callback inputs."""

from pathlib import Path


ROOT = Path(__file__).parents[2] / "lib" / "Xm"

for name in ("TextSel.c", "TextFSel.c"):
    source = (ROOT / name).read_text()
    for marker in ("HandleTargets(Widget w", "DoStuff(Widget w"):
        start = source.rindex(marker)
        end = source.find("\nstatic ", start)
        body = source[start:] if end == -1 else source[start:end]
        assert "w == NULL" in body, (name, marker)
        assert "ds == NULL" in body, (name, marker)
        if marker.startswith("DoStuff"):
            assert "closure == NULL" in body, (name, marker)

print("Text selection callbacks validate inputs")
