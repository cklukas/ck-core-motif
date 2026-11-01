#!/usr/bin/env python3
"""Generate the gallery's manifest registry and source-tab fragments.

The JSON manifest is the only component inventory.  Source text is extracted
from annotated gallery_examples.c rather than maintained as UI strings.
"""
from __future__ import annotations

import json
import hashlib
import sys
from pathlib import Path

from gallery_source import GallerySource


def fail(message: str) -> "NoReturn":
    raise SystemExit(f"gallery asset generation: {message}")


def main() -> int:
    if len(sys.argv) != 4:
        fail("usage: generate-gallery-assets.py MANIFEST EXAMPLES OUT_DIR")
    manifest_path, examples_path, out_dir = map(Path, sys.argv[1:])
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    entries = manifest.get("entries")
    if not isinstance(entries, list) or not entries:
        fail("manifest has no entries")
    source = examples_path.read_text(encoding="utf-8")
    excerpts = GallerySource(source)
    for entry in entries:
        required = ("id", "class_symbol", "category", "constructible",
                    "source_fragment", "interaction_scenario", "docs_anchor",
                    "screenshot_scenario")
        missing = [field for field in required if field not in entry]
        if missing:
            fail(f"{entry.get('id', '<unknown>')} missing {', '.join(missing)}")

    out_dir.mkdir(parents=True, exist_ok=True)
    header = ["/* Generated; do not edit. */", "#ifndef CKMOTIF_GALLERY_MANIFEST_H",
              "#define CKMOTIF_GALLERY_MANIFEST_H", "",
              "#ifdef PRINTING_SUPPORTED",
              "#define CKMOTIF_GALLERY_PRINT_CLASS xmPrintShellWidgetClass",
              "#else",
              "#define CKMOTIF_GALLERY_PRINT_CLASS NULL",
              "#endif", "", "typedef struct {",
              "    const char *id;", "    const char *label;",
              "    const char *category;", "    const char *class_symbol;",
              "    const char *kind;", "    WidgetClass class;",
              "    int popup_shell;",
              "    const char *source_fragment;", "    const char *interaction;",
              "    const char *docs_anchor;", "    const char *screenshot;",
              "    int constructible;", "} GalleryManifestEntry;", "",
              "static GalleryManifestEntry gallery_manifest[] = {"]
    for entry in entries:
        label = entry["id"].split(".", 1)[-1]
        values = [entry["id"], label, entry["category"], entry["class_symbol"],
                  entry["kind"],
                  entry["source_fragment"], entry["interaction_scenario"],
                  entry["docs_anchor"], entry["screenshot_scenario"]]
        class_value = "NULL"
        # The class pointer follows the class name, before the acceptance IDs.
        popup_shell = int(entry["constructible"] and "Shell" in entry["class_symbol"])
        parts = (values[:5] + [class_value, str(popup_shell)] + values[5:] +
                 [str(int(entry["constructible"]))])
        rendered = []
        for index, part in enumerate(parts):
            if index in (5, 6) or index == len(parts) - 1:
                rendered.append(part)
            else:
                rendered.append('"' + part.replace('"', '\\"') + '"')
        header.append(f"    {{{', '.join(rendered)}}},")
    categories = list(dict.fromkeys(entry["category"] for entry in entries))
    header += ["};", "", "static const char *gallery_categories[] = {"]
    header += [f'    "{category}",' for category in categories]
    header += ["};", f"#define GALLERY_CATEGORY_COUNT {len(categories)}",
               "#define GALLERY_MANIFEST_COUNT \\",
               "    (sizeof(gallery_manifest) / sizeof(gallery_manifest[0]))", "",
              "static void gallery_manifest_initialize(void)"]
    header.append("{")
    header += ["#if defined(__GNUC__) || defined(__clang__)",
               "#pragma GCC diagnostic push",
               "#pragma GCC diagnostic ignored \"-Wdeprecated-declarations\""]
    for index, entry in enumerate(entries):
        if not entry["constructible"]:
            continue
        class_value = ("CKMOTIF_GALLERY_PRINT_CLASS" if
                       entry["header"] == "lib/Xm/Print.h" else entry["class_symbol"])
        header.append(f"    gallery_manifest[{index}].class = {class_value};")
    header += ["}", "#pragma GCC diagnostic pop", "#endif", "", "#endif"]
    (out_dir / "gallery_manifest.h").write_text("\n".join(header) + "\n", encoding="utf-8")

    sources = ["/* Generated; do not edit. */", "#ifndef CKMOTIF_GALLERY_SOURCES_H",
               "#define CKMOTIF_GALLERY_SOURCES_H", "", "typedef struct {",
               "    const char *fragment;", "    const char *text;",
               "} GallerySourceFragment;", "", "static const GallerySourceFragment gallery_sources[] = {"]
    for entry in entries:
        text = excerpts.text(entry)
        text = text.replace("\\", "\\\\").replace('"', '\\"').replace("\n", "\\n")
        sources.append(f'    {{"{entry["source_fragment"]}", "{text}"}},')
    sources += ["};", "#endif"]
    (out_dir / "gallery_sources.h").write_text("\n".join(sources) + "\n", encoding="utf-8")

    provenance = {
        "schema": 1,
        "manifest_sha256": hashlib.sha256(manifest_path.read_bytes()).hexdigest(),
        "examples_sha256": hashlib.sha256(examples_path.read_bytes()).hexdigest(),
        "entries": [],
    }
    for entry in entries:
        provenance["entries"].append({
            "id": entry["id"],
            "category": entry["category"],
            "class_symbol": entry["class_symbol"],
            "constructible": entry["constructible"],
            "example": entry["construction_owner"],
            "source_fragment": entry["source_fragment"],
            "interaction_scenario": entry["interaction_scenario"],
            "docs_anchor": entry["docs_anchor"],
            "screenshot_scenario": entry["screenshot_scenario"],
            "capability": "optional-printing" if entry["header"] == "lib/Xm/Print.h"
                           else "standard",
            "qualification": "scaffolded" if entry["constructible"] else "excluded",
        })
    (out_dir / "gallery_provenance.json").write_text(
        json.dumps(provenance, indent=2, sort_keys=True) + "\n", encoding="utf-8"
    )
    return 0


if __name__ == "__main__":
    main()
