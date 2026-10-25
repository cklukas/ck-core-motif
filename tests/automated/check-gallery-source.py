#!/usr/bin/env python3
"""Check that the gallery registry and source fragments remain manifest-driven."""
from __future__ import annotations

import json
import re
import sys
from pathlib import Path


def main() -> int:
    root = Path(__file__).resolve().parents[2]
    manifest = json.loads((root / "docs/modern-gallery-manifest.json").read_text())
    entries = manifest["entries"]
    generator = root / "demos/programs/ckmotif-gallery/generate-gallery-assets.py"
    example = root / "demos/programs/ckmotif-gallery/gallery_examples.c"
    text = example.read_text()
    if text.count("GALLERY-EXAMPLE-BEGIN") != text.count("GALLERY-EXAMPLE-END"):
        print("unbalanced gallery source annotations", file=sys.stderr)
        return 1
    for marker in ("shared-public-construction", "shell-public-construction"):
        if not re.search(rf"GALLERY-EXAMPLE-BEGIN: {marker}", text):
            print(f"missing {marker} source annotation", file=sys.stderr)
            return 1
    if "GALLERY-EXAMPLE-BEGIN: public-teardown" not in text:
        print("missing public teardown source annotation", file=sys.stderr)
        return 1
    if ("XtVaCreateManagedWidget" not in text or "XtAddCallback" not in text or
            "XmNvalueChangedCallback" not in text or "XmNhelpCallback" not in text):
        print("source example is not an interactive public-API example", file=sys.stderr)
        return 1
    gallery = (root / "demos/programs/ckmotif-gallery/gallery.c").read_text()
    source_contract = gallery + "\n" + text
    for api in ("XmSidebar", "XmTreeView", "Notebook.h", "XmStatusBar",
                "Example", "Source", "XmTreeViewSetSelectedId",
                "XmSidebarSetExpanded", "XmNcurrentPageNumber",
                "XmAccessibilityCreate", "XmAccessibilityQuery",
                "exercise_component_matrix", "XtCallCallbacks",
                "XmTextGetString", "XmApplyAppearanceProfile",
                "XmSetDisplayScaleOverride", "XmClearDisplayScaleOverride",
                "XmDragStart", "XmNinputCallback"):
        if api not in source_contract:
            print(f"gallery shell is missing {api}", file=sys.stderr)
            return 1
    for contract in ("parse_appearance", "parse_scale", "XmApplyAppearanceProfile",
                     "XmSetDisplayScaleOverride", "XSync(XtDisplay(shell), False)",
                     "--appearance requires a profile name",
                     "--scale requires a numeric value"):
        if contract not in gallery:
            print(f"gallery screenshot configuration is missing {contract}",
                  file=sys.stderr)
            return 1
    for field in ("source_fragment", "interaction", "docs_anchor", "screenshot"):
        if f"entry->{field}" not in gallery:
            print(f"gallery does not consume {field} metadata", file=sys.stderr)
            return 1
    if "gallery_destroy_component_example(&state->component)" not in gallery:
        print("gallery teardown is not routed through the annotated example code", file=sys.stderr)
        return 1
    if "--integration-test" not in (root / "tests/automated/run-gallery-integration.py").read_text():
        print("native integration runner is not wired to the gallery", file=sys.stderr)
        return 1
    screenshot_runner = root / "tests/automated/run-gallery-screenshots.py"
    screenshot_source = screenshot_runner.read_text()
    if "--component" not in screenshot_source:
        print("screenshot runner cannot select manifest components", file=sys.stderr)
        return 1
    if '"--appearance", args.appearance' not in screenshot_source or \
            '"--scale", str(args.scale)' not in screenshot_source:
        print("screenshot runner cannot select profile and scale", file=sys.stderr)
        return 1
    if "--component" not in gallery:
        print("gallery does not expose deterministic component selection", file=sys.stderr)
        return 1
    if "check-gallery-provenance.py" not in (root / "demos/programs/ckmotif-gallery/Makefile.am").read_text():
        print("gallery provenance is not wired into make check", file=sys.stderr)
        return 1
    if "PRINTING_SUPPORTED" not in (root / "demos/programs/ckmotif-gallery/generate-gallery-assets.py").read_text():
        print("print-only capability is not conditional", file=sys.stderr)
        return 1
    for entry in entries:
        for key in ("source_fragment", "interaction_scenario", "docs_anchor",
                    "screenshot_scenario"):
            if not entry.get(key):
                print(f"{entry['id']} lacks {key}", file=sys.stderr)
                return 1
    private_prefix = "_" + "Xm"
    if private_prefix in (root / "demos/programs/ckmotif-gallery/gallery.c").read_text():
        print("gallery uses a private _Xm interface", file=sys.stderr)
        return 1
    print(f"gallery source contract: {len(entries)} manifest entries covered")
    print(f"generator: {generator}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
