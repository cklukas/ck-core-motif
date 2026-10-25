#!/usr/bin/env python3
"""Guard MenuShell composite traversals against signed indices."""

from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "MenuShell.c").read_text(
    encoding="utf-8"
)
for marker in ("_XmFastExpose(", "PopupSharedMenuShell(", "ChangeManaged("):
    start = text.index(marker, 10000)
    end = text.find("\nstatic ", start + len(marker))
    if end < 0:
        end = len(text)
    body = text[start:end]
    if "composite.num_children" not in body:
        raise SystemExit(f"{marker} no longer traverses children")
    if "register int i;" in body or "   int i;" in body:
        raise SystemExit(f"{marker} uses a signed child index")

print("MenuShell child-count bounds checks passed")
