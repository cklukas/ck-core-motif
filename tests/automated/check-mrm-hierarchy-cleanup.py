#!/usr/bin/env python3
"""Check partial Mrm hierarchy construction releases owned state."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmhier.c"
text = source.read_text(encoding="utf-8")
start = text.index("Urm__OpenHierarchy (")
end = text.index("\n\n\f", start)
region = text[start:end]

for needle in (
    "hiptr->file_list = NULL;",
    "hiptr->name_registry = NULL;",
    "hiptr->grp_ids[ndx] = NULL;",
    "if ( hiptr->file_list == NULL )",
    "if ( idvec == NULL )",
):
    if needle not in region:
        raise SystemExit(f"hierarchy allocation initialization missing: {needle}")

if region.count("Urm__CloseHierarchy (hiptr)") < 4:
    raise SystemExit("partial hierarchy failures do not consistently clean up")
if "UrmFreeResourceContext (class_ctx);" not in region:
    raise SystemExit("resource-context failure does not release class context")
if region.count("UrmFreeResourceContext (resource_ctx);") < 3:
    raise SystemExit("resource-table failure paths do not release resource context")

print("Mrm hierarchy construction cleans up partial failures")
