#!/usr/bin/env python3
"""Generate the manifest-backed component catalogue used by the gallery docs."""
from __future__ import annotations

import json
import hashlib
import sys
import re
from pathlib import Path


sys.path.insert(0, str(Path(__file__).resolve().parents[2] /
                       "demos/programs/ckmotif-gallery"))
from gallery_source import GallerySource


def source_excerpt(root: Path, entry: dict) -> list[str]:
    """Use the same exact source selection as the gallery's Source tab."""
    relative = "demos/programs/ckmotif-gallery/gallery_examples.c"
    excerpts = GallerySource((root / relative).read_text())
    lines = [excerpts.context(entry), ""]
    for part in excerpts.excerpts(entry):
        lines.extend([f"<<< @/{relative}#L{part.first}-L{part.last}", ""])
    if not entry["constructible"]:
        lines.extend([f"<<< @/{entry['header']}", ""])
    lines.extend([f"[Complete demo source](../{relative}) · [Public header](../{entry['header']})", ""])
    return lines


def main() -> int:
    if len(sys.argv) not in (3, 4) or (len(sys.argv) == 4 and sys.argv[3] != "--reference-pages"):
        raise SystemExit("usage: generate-gallery-catalogue.py MANIFEST OUTPUT [--reference-pages]")
    manifest_path, output_path = map(Path, sys.argv[1:3])
    root = Path(__file__).resolve().parents[2]
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
        "Use **Demo** to inspect the captured application and **Source** for its actual construction code.",
        "Screenshots retain their recorded capture version; they are not a claim of completed visual redesign.",
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
                "::: tabs",
                "== Demo",
                "",
                f"![{entry['class_symbol']} in the component demo](gallery-screenshots/{entry['id'].replace('.', '-')}.png)",
                "",
                "== Source",
                "",
                *source_excerpt(root, entry),
                ":::",
                "",
            ])
    if "--reference-pages" in sys.argv:
        for entry in entries:
            name = re.sub(r"(Widget|Gadget)?Class$", "", entry["class_symbol"][2:])
            slug = re.sub(r"(?<!^)(?=[A-Z])", "-", name).lower()
            page = root / "docs" / (slug + ".md")
            if not page.exists():
                continue
            start_marker = "<!-- gallery-demo-tabs: begin -->"
            end_marker = "<!-- gallery-demo-tabs: end -->"
            block = "\n".join([start_marker, "## Component demo", "",
                "Captured gallery example and its corresponding source excerpt.", "",
                "::: tabs", "== Demo", "",
                f"![{entry['class_symbol']} demo](gallery-screenshots/{entry['id'].replace('.', '-')}.png)",
                "", "== Source", "", *source_excerpt(root, entry), ":::", end_marker, "", ""])
            original = page.read_text()
            if start_marker in original:
                begin = original.index(start_marker)
                end = original.index(end_marker, begin) + len(end_marker)
                updated = original[:begin] + block.rstrip() + original[end:]
            else:
                position = original.find("\n## ")
                if position < 0: position = len(original)
                updated = original[:position].rstrip() + "\n\n" + block + original[position:].lstrip()
            page.write_text(updated.rstrip() + "\n")
    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text("\n".join(lines), encoding="utf-8")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
