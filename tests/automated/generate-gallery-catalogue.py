#!/usr/bin/env python3
"""Generate the manifest-backed component catalogue used by the gallery docs."""
from __future__ import annotations

import json
import hashlib
import sys
from pathlib import Path


def main() -> int:
    if len(sys.argv) != 3:
        raise SystemExit("usage: generate-gallery-catalogue.py MANIFEST OUTPUT")
    manifest_path, output_path = map(Path, sys.argv[1:])
    entries = json.loads(manifest_path.read_text(encoding="utf-8"))["entries"]
    categories: dict[str, list[dict[str, object]]] = {}
    for entry in entries:
        categories.setdefault(entry["category"], []).append(entry)

    lines = [
        "# CK Core Motif component catalogue",
        "",
        "<!-- Generated from docs/modern-gallery-manifest.json; do not edit. -->",
        f"<!-- manifest-sha256: {hashlib.sha256(manifest_path.read_bytes()).hexdigest()} -->",
        "",
        "This catalogue is generated from the gallery manifest. Each anchor is",
        "the documentation target recorded by the corresponding manifest entry.",
        "",
    ]
    for category, category_entries in categories.items():
        lines.extend([f"## {category}", ""])
        for entry in category_entries:
            lines.extend([
                f"<a id=\"{entry['docs_anchor']}\"></a>",
                f"### `{entry['id']}`",
                "",
                f"- Class: `{entry['class_symbol']}`",
                f"- Example: `{entry['source_fragment']}`",
                f"- Interaction: `{entry['interaction_scenario']}`",
                f"- Screenshot: `{entry['screenshot_scenario']}`",
                f"- Capability: {'constructible' if entry['constructible'] else 'inventory-only'}",
                "",
                f"![Gallery screenshot](gallery-screenshots/{entry['id'].replace('.', '-')}.png)",
                "",
            ])
    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text("\n".join(lines), encoding="utf-8")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
