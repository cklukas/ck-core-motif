#!/usr/bin/env python3
"""Ensure topical documentation overviews and their links are shipped."""

from pathlib import Path
import re

root = Path(__file__).parents[2]
docs = root / "docs"
nav = (root / "ckdocs.yml").read_text(encoding="utf-8")

for overview in ("maintenance-overview.md", "mwm-overview.md"):
    path = docs / overview
    text = path.read_text(encoding="utf-8")
    links = re.findall(r"\]\(([^)#]+\.md)\)", text)
    if not links:
        raise SystemExit(f"{overview} has no Markdown links")
    for link in links:
        if not (docs / link).is_file():
            raise SystemExit(f"{overview} links to missing {link}")
    if overview not in nav:
        raise SystemExit(f"{overview} is missing from ckdocs.yml navigation")

print("topical documentation overviews and links present")
