#!/usr/bin/env python3
"""Verify published gallery PNGs and their manifest-bound provenance."""
from __future__ import annotations

import hashlib
import json
from pathlib import Path
import struct


root = Path(__file__).resolve().parents[2]
manifest_path = root / "docs/modern-gallery-manifest.json"
catalogue_path = root / "docs/modern-gallery-components.md"
publication_path = root / "docs/gallery-screenshots/manifest.json"
manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
publication = json.loads(publication_path.read_text(encoding="utf-8"))
expected_hash = hashlib.sha256(manifest_path.read_bytes()).hexdigest()
assert publication["manifest_sha256"] == expected_hash
assert publication["capture"]["result"] == "pass"
assert publication["capture"]["appearance_profile"] == "modern-light"
assert publication["capture"]["dpi_input"]
expected_ids = [entry["id"] for entry in manifest["entries"]]
actual_ids = [entry["id"] for entry in publication["entries"]]
assert actual_ids == expected_ids
catalogue = catalogue_path.read_text(encoding="utf-8")
for entry in publication["entries"]:
    path = publication_path.parent / entry["png"]
    assert path.is_file(), path
    assert hashlib.sha256(path.read_bytes()).hexdigest() == entry["sha256"]
    data = path.read_bytes()
    assert data[:8] == b"\x89PNG\r\n\x1a\n" and data[12:16] == b"IHDR"
    width, height = struct.unpack(">II", data[16:24])
    assert (width, height) == (entry["width"], entry["height"])
    assert f"gallery-screenshots/{entry['png']}" in catalogue
print(f"PASS gallery screenshot assets: {len(actual_ids)} PNGs verified")
