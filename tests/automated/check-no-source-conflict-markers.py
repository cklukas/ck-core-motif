#!/usr/bin/env python3
"""Reject unresolved version-control conflict markers in C sources."""

from pathlib import Path

root = Path(__file__).parents[2]
markers = ("<<<<<<<", ">>>>>>>")
for path in root.rglob("*"):
    if path.suffix not in {".c", ".h", ".cc", ".cpp"} or not path.is_file():
        continue
    text = path.read_text(encoding="utf-8", errors="replace")
    for line_number, line in enumerate(text.splitlines(), 1):
        if line.startswith(markers):
            raise SystemExit(f"unresolved source conflict marker: {path}:{line_number}")

print("PASS C/C++ sources contain no unresolved conflict markers")
