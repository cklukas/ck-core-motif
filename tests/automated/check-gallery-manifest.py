#!/usr/bin/env python3
"""Validate the checked-in component inventory for the modern gallery.

The inventory is deliberately checked against XmAll.h rather than against a
hand-maintained list of guessed widgets.  Public class declarations are
therefore either assigned a future gallery example or explicitly recorded as
an abstract/protocol exclusion.  The verifier is useful before the gallery
exists: it detects header drift and duplicate component identities early.
"""

from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path


CLASS_RE = re.compile(r"\bxm[A-Za-z0-9_]+(?:Widget|Gadget|Object)?Class\b")
INCLUDE_RE = re.compile(r"^\s*#include\s+<Xm/([^>]+)>")
PUBLIC_SUFFIXES = ("P.h", "I.h", "T.h")
NON_COMPONENT_HEADERS = {
    "Appearance.h",
    "AtomMgr.h",
    "CutPaste.h",
    "DragDrop.h",
    "MwmUtil.h",
    "Protocols.h",
    "RepType.h",
    "Transfer.h",
    "TxtPropCv.h",
    "VirtKeys.h",
    "Xm.h",
    "XmIm.h",
    "XmStrDefs.h",
    "Xmfuncs.h",
}
BASE_CLASSES = {
    "xmDisplayClass",
    "xmGadgetClass",
    "xmManagerWidgetClass",
    "xmPrimitiveWidgetClass",
    "xmScreenClass",
}
NONCONSTRUCTIBLE_CLASSES = {
    "xmDragContextClass",
    "xmSlideContextClass",
}
CATEGORIES = {
    "layout",
    "commands-navigation",
    "buttons-selection",
    "text-input",
    "data-views",
    "feedback-dialogs",
    "media",
}


def public_inventory(root: Path) -> list[tuple[str, str]]:
    all_header = root / "lib/Xm/XmAll.h"
    headers: list[str] = []
    for line in all_header.read_text(encoding="utf-8").splitlines():
        match = INCLUDE_RE.match(line)
        if not match:
            continue
        header = match.group(1)
        if header.endswith(PUBLIC_SUFFIXES) or header in NON_COMPONENT_HEADERS:
            continue
        path = root / "lib/Xm" / header
        if path.is_file() and header not in headers:
            headers.append(header)

    inventory: list[tuple[str, str]] = []
    for header in headers:
        source = (root / "lib/Xm" / header).read_text(encoding="utf-8")
        for symbol in sorted(set(CLASS_RE.findall(source))):
            inventory.append((header, symbol))
    return inventory


def expected_kind(symbol: str) -> tuple[str, bool]:
    if symbol in BASE_CLASSES:
        return "base", False
    if "Object" in symbol or symbol in NONCONSTRUCTIBLE_CLASSES:
        return "object", False
    if "Gadget" in symbol:
        return "gadget", True
    return "widget", True


def category_for_header(header: str) -> str:
    name = header.lower()
    if any(
        token in name
        for token in (
            "form",
            "column",
            "container",
            "frame",
            "mainw",
            "manager",
            "notebook",
            "paned",
            "rowcolumn",
            "scrolled",
            "tabbox",
            "tabstack",
            "toolbar",
            "sidebar",
            "statusbar",
            "hierarchy",
            "tree",
            "iconbox",
            "divider",
        )
    ):
        return "layout"
    if any(
        token in name
        for token in (
            "command",
            "menu",
            "cascade",
            "dialog",
            "message",
            "selection",
            "filesb",
            "grabshell",
            "dropdown",
            "combobox",
            "breadcrumb",
            "flyout",
            "teaching",
            "toast",
            "link",
        )
    ):
        return "commands-navigation"
    if any(
        token in name
        for token in (
            "arrow",
            "button",
            "toggle",
            "push",
            "segment",
            "rating",
            "colorpicker",
            "list",
            "spin",
            "step",
            "scale",
            "scrollbar",
        )
    ):
        return "buttons-selection"
    if any(token in name for token in ("label", "text", "dataf", "search", "prompt", "autosuggest")):
        return "text-input"
    if any(token in name for token in ("datagrid", "multilist", "calendar", "datepicker", "timepicker", "outline")):
        return "data-views"
    if any(
        token in name
        for token in (
            "infobar",
            "emptystate",
            "skeleton",
            "progress",
            "spinner",
            "numberbox",
            "badge",
            "avatar",
            "chip",
            "iconlabel",
        )
    ):
        return "feedback-dialogs"
    if any(token in name for token in ("drawing", "icong", "iconh", "iconbutton", "drop")):
        return "media"
    return "layout"


def template(root: Path) -> dict[str, object]:
    entries = []
    for header, symbol in public_inventory(root):
        kind, constructible = expected_kind(symbol)
        stem = symbol.removeprefix("xm").lower()
        entries.append(
            {
                "id": f"xm.{stem}",
                "header": f"lib/Xm/{header}",
                "class_symbol": symbol,
                "kind": kind,
                "constructible": constructible,
                "category": category_for_header(header),
                "construction_owner": f"gallery.component.{stem}",
                "source_fragment": f"gallery.source.{stem}",
                "interaction_scenario": f"gallery.interaction.{stem}",
                "docs_anchor": f"gallery.component.{stem}",
                "screenshot_scenario": f"gallery.screenshot.{stem}",
                "disposition": "planned-example"
                if constructible
                else f"excluded-{kind}",
            }
        )
    return {
        "schema": 1,
        "source": "lib/Xm/XmAll.h",
        "generated_by": "tests/automated/check-gallery-manifest.py",
        "entries": entries,
    }


def validate(root: Path, manifest_path: Path) -> None:
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    if manifest.get("schema") != 1:
        raise ValueError("manifest schema must be 1")
    if manifest.get("source") != "lib/Xm/XmAll.h":
        raise ValueError("manifest source must be lib/Xm/XmAll.h")

    inventory = public_inventory(root)
    expected = set(inventory)
    entries = manifest.get("entries")
    if not isinstance(entries, list):
        raise ValueError("manifest entries must be a list")

    seen_pairs: set[tuple[str, str]] = set()
    seen_ids: set[str] = set()
    required = {
        "id",
        "header",
        "class_symbol",
        "kind",
        "constructible",
        "category",
        "construction_owner",
        "source_fragment",
        "interaction_scenario",
        "docs_anchor",
        "screenshot_scenario",
        "disposition",
    }
    for index, entry in enumerate(entries):
        if not isinstance(entry, dict):
            raise ValueError(f"entry {index} is not an object")
        missing = required - entry.keys()
        if missing:
            raise ValueError(f"entry {index} missing {sorted(missing)}")
        pair = (Path(entry["header"]).name, entry["class_symbol"])
        if pair not in expected:
            raise ValueError(f"entry {index} is not declared by XmAll.h: {pair}")
        if pair in seen_pairs:
            raise ValueError(f"duplicate header/class entry: {pair}")
        seen_pairs.add(pair)
        if entry["id"] in seen_ids:
            raise ValueError(f"duplicate component id: {entry['id']}")
        seen_ids.add(entry["id"])
        if entry["category"] not in CATEGORIES:
            raise ValueError(f"entry {index} has unknown category")
        if not isinstance(entry["constructible"], bool):
            raise ValueError(f"entry {index} constructible must be boolean")
        kind, constructible = expected_kind(entry["class_symbol"])
        if entry["kind"] != kind or entry["constructible"] != constructible:
            raise ValueError(f"entry {index} has incorrect kind/constructibility")
        if entry["constructible"] and entry["disposition"] != "planned-example":
            raise ValueError(f"constructible entry {index} must be planned-example")
        if not entry["constructible"] and not str(entry["disposition"]).startswith(
            "excluded-"
        ):
            raise ValueError(f"non-constructible entry {index} must be excluded")
        for field in required - {"constructible"}:
            if not isinstance(entry[field], str) or not entry[field].strip():
                raise ValueError(f"entry {index} field {field} must be non-empty text")

    missing = expected - seen_pairs
    if missing:
        raise ValueError(f"manifest omitted {len(missing)} XmAll.h declarations: {sorted(missing)}")
    if len(entries) != len(inventory):
        raise ValueError("manifest entry count does not match XmAll.h inventory")
    constructible = sum(bool(entry["constructible"]) for entry in entries)
    excluded = len(entries) - constructible
    print(
        f"PASS gallery manifest: {len(entries)} public classes "
        f"({constructible} constructible, {excluded} explicit exclusions)"
    )


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-root", type=Path, default=Path(__file__).parents[2])
    parser.add_argument("--manifest", type=Path)
    parser.add_argument("--print-template", action="store_true")
    args = parser.parse_args()
    root = args.source_root.resolve()
    if args.print_template:
        print(json.dumps(template(root), indent=2, sort_keys=False))
        return 0
    manifest = args.manifest or root / "docs/modern-gallery-manifest.json"
    try:
        validate(root, manifest)
    except (OSError, ValueError, json.JSONDecodeError) as exc:
        print(f"gallery manifest failure: {exc}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
