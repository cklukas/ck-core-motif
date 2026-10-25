#!/usr/bin/env python3
"""Verify the generated gallery provenance is a complete manifest projection."""
from __future__ import annotations

import hashlib
import json
import re
import sys
from pathlib import Path


FIELDS = (
    "id", "category", "class_symbol", "constructible", "example",
    "source_fragment", "interaction_scenario", "docs_anchor",
    "screenshot_scenario", "capability", "qualification",
)


def fail(message: str) -> "NoReturn":
    raise SystemExit(f"gallery provenance: {message}")


def main() -> int:
    if len(sys.argv) not in (4, 5):
        fail("usage: check-gallery-provenance.py MANIFEST EXAMPLES PROVENANCE [CATALOGUE]")
    manifest_path, examples_path, provenance_path = map(Path, sys.argv[1:4])
    catalogue_path = Path(sys.argv[4]) if len(sys.argv) == 5 else None
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    provenance = json.loads(provenance_path.read_text(encoding="utf-8"))
    expected = manifest.get("entries")
    actual = provenance.get("entries")
    if not isinstance(expected, list) or not isinstance(actual, list):
        fail("manifest and provenance must contain entry lists")
    if provenance.get("manifest_sha256") != hashlib.sha256(manifest_path.read_bytes()).hexdigest():
        fail("manifest hash is stale")
    if provenance.get("examples_sha256") != hashlib.sha256(examples_path.read_bytes()).hexdigest():
        fail("annotated source hash is stale")
    if catalogue_path is not None:
        catalogue = catalogue_path.read_text(encoding="utf-8")
        manifest_hash = hashlib.sha256(manifest_path.read_bytes()).hexdigest()
        if f"manifest-sha256: {manifest_hash}" not in catalogue:
            fail("catalogue manifest hash is stale")
        expected_anchors = [entry["docs_anchor"] for entry in expected]
        actual_anchors = re.findall(r'<a id="([^"]+)"></a>', catalogue)
        if len(actual_anchors) != len(set(actual_anchors)):
            fail("duplicate catalogue documentation anchors")
        if set(actual_anchors) != set(expected_anchors):
            fail("catalogue documentation anchors differ from manifest")
    if len(expected) != len(actual):
        fail(f"entry count differs: manifest={len(expected)} provenance={len(actual)}")
    by_id = {item.get("id"): item for item in actual}
    if len(by_id) != len(actual):
        fail("duplicate provenance IDs")
    for entry in expected:
        item = by_id.get(entry.get("id"))
        if item is None:
            fail(f"missing {entry['id']}")
        for field in FIELDS:
            if field not in item or item[field] in (None, ""):
                fail(f"{entry['id']} missing {field}")
        if item["source_fragment"] != entry["source_fragment"]:
            fail(f"{entry['id']} source fragment drift")
        if item["interaction_scenario"] != entry["interaction_scenario"]:
            fail(f"{entry['id']} interaction scenario drift")
        if item["docs_anchor"] != entry["docs_anchor"]:
            fail(f"{entry['id']} docs anchor drift")
        if item["screenshot_scenario"] != entry["screenshot_scenario"]:
            fail(f"{entry['id']} screenshot scenario drift")
    suffix = " and catalogue anchors" if catalogue_path is not None else ""
    print(f"gallery provenance: {len(actual)} entries verified{suffix}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
