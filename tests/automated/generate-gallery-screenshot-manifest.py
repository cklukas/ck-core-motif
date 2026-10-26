#!/usr/bin/env python3
"""Project a passing gallery capture receipt into publishable asset metadata."""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import struct


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def png_size(path: Path) -> tuple[int, int]:
    data = path.read_bytes()
    if data[:8] != b"\x89PNG\r\n\x1a\n" or data[12:16] != b"IHDR":
        raise ValueError(f"not a PNG: {path}")
    width, height = struct.unpack(">II", data[16:24])
    if not width or not height:
        raise ValueError(f"empty PNG dimensions: {path}")
    return width, height


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("receipt", type=Path)
    parser.add_argument("output", type=Path)
    parser.add_argument("--viewport", required=True)
    parser.add_argument("--profile", default="classic")
    parser.add_argument("--dpi-input", default="server-default")
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[2]
    manifest_path = root / "docs/modern-gallery-manifest.json"
    asset_root = root / "docs/gallery-screenshots"
    receipt = json.loads(args.receipt.read_text(encoding="utf-8"))
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    expected_hash = sha256(manifest_path)
    if receipt.get("result") != "pass" or not receipt.get("success"):
        raise SystemExit("screenshot receipt is not a passing qualification")
    if receipt.get("manifest_sha256") != expected_hash:
        raise SystemExit("screenshot receipt manifest hash is stale")
    if receipt.get("appearance") != args.profile:
        raise SystemExit("published profile differs from the captured profile")
    by_id = {entry["id"]: entry for entry in receipt.get("entries", [])}
    expected_ids = [entry["id"] for entry in manifest["entries"]]
    if list(by_id) != expected_ids:
        raise SystemExit("screenshot receipt entries differ from the manifest")
    entries = []
    for entry_id in expected_ids:
        source = by_id[entry_id]
        path = asset_root / Path(source["png"]).name
        if not path.is_file():
            raise SystemExit(f"missing published screenshot: {path}")
        if sha256(path) != source["png_sha256"]:
            raise SystemExit(f"published screenshot differs from captured bytes: {path}")
        width, height = png_size(path)
        entries.append({
            "id": entry_id,
            "png": path.name,
            "sha256": sha256(path),
            "width": width,
            "height": height,
        })
    output = {
        "schema": 1,
        "manifest_sha256": expected_hash,
        "binary_sha256": receipt["binary_sha256"],
        "library_sha256": receipt.get("library_sha256"),
        "capture": {
            "result": receipt["result"],
            "display": receipt["display"],
            "window_name": receipt["window_name"],
            "viewport": args.viewport,
            "appearance_profile": args.profile,
            "scale": receipt["scale"],
            "dpi_input": args.dpi_input,
            "runner": "tests/automated/run-gallery-screenshots.py",
        },
        "entries": entries,
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(output, indent=2) + "\n", encoding="utf-8")
    print(f"gallery screenshot manifest: {len(entries)} assets recorded")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
