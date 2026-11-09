#!/usr/bin/env python3
"""Check Mrm hierarchy opening validates public arguments before narrowing."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmappl.c").read_text(
    encoding="utf-8"
)
open_start = source.index("MrmOpenHierarchy (\n")
per_display_start = source.index("MrmOpenHierarchyPerDisplay (Display")
per_display_end = source.index("Cardinal\nMrmOpenHierarchyFromBuffer", per_display_start)
open_region = source[open_start:per_display_start]
per_display_region = source[per_display_start:per_display_end]

guard = "if (os_ext_list != NULL && *os_ext_list == NULL)"
assert guard in per_display_region
assert "_MrmProcessUnlock();" in per_display_region[per_display_region.index(guard) : per_display_region.index("if (os_ext_list == NULL)")]
assert "return MrmFAILURE;" in per_display_region[per_display_region.index(guard) : per_display_region.index("if (os_ext_list == NULL)")]
assert per_display_region.index(guard) < per_display_region.index("(*os_ext_list)->display = display;")

helper = "Mrm__ValidLegacyCount (int count)"
assert helper in source
assert "count >= 0 && count <= SHRT_MAX" in source[source.index(helper):open_start]
for region in (open_region, per_display_region):
    check = "if (!Mrm__ValidLegacyCount(num_files))"
    assert check in region
    assert "return MrmFAILURE;" in region[region.index(check) : region.index("_MrmProcessLock();")]
    assert region.index(check) < region.index("Urm__OpenHierarchy")
    assert "Urm__OpenHierarchy" in region
    assert "(MrmCount)num_files" in region[region.index("Urm__OpenHierarchy"):]

register_start = source.index("MrmRegisterNames (MrmRegisterArglist")
register_hierarchy_start = source.index("MrmRegisterNamesInHierarchy (MrmHierarchy")
register_hierarchy_end = source.index(
    "MrmFetchInterfaceModule (MrmHierarchy", register_hierarchy_start
)
for region in (
    source[register_start:register_hierarchy_start],
    source[register_hierarchy_start:register_hierarchy_end],
):
    check = "if (!Mrm__ValidLegacyCount(num_reg))"
    assert check in region
    assert "return MrmFAILURE;" in region[region.index(check):]
    assert region.index(check) < region.index("_MrmProcessLock();")
    assert "(MrmCount)num_reg" in region

print("PASS Mrm public entry points validate OS parameters and wide counts")
