#!/usr/bin/env python3
"""Check FormSection and Skeleton preserve labels on copy failure."""

from pathlib import Path


root = Path(__file__).parents[2] / "lib" / "Xm"
form = (root / "FormSection.c").read_text(encoding="utf-8")
start = form.rindex("static Boolean SetValues(")
end = form.index("\nstatic XtGeometryResult GeometryManager", start)
region = form[start:end]
for field in ("header_text", "footer_text"):
    if f"XmString replacement = copy_string(section->section.{field});" not in region:
        raise SystemExit(f"FormSection missing {field} replacement validation")
    if f"section->section.{field} = old->section.{field};" not in region:
        raise SystemExit(f"FormSection does not preserve {field}")

skeleton = (root / "Skeleton.c").read_text(encoding="utf-8")
start = skeleton.rindex("static Boolean SetValues(")
region = skeleton[start : skeleton.index("\nstatic XtGeometryResult QueryGeometry", start)]
for needle in ("XmString replacement = copy_string", "skeleton->skeleton.label = replacement"):
    if needle not in region:
        raise SystemExit(f"Skeleton missing transactional label update: {needle}")

print("PASS FormSection and Skeleton label updates are transactional")
