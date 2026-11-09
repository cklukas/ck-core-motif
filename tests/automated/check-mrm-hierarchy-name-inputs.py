from pathlib import Path
import os

source = Path(os.environ.get(
    "MOTIF_SOURCE", Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmhier.c"))
text = source.read_text()
reg_start = text.index("Urm__RegisterNamesInHierarchy (MrmHierarchy")
reg_end = text.index("Urm__LookupNameInHierarchy (MrmHierarchy", reg_start)
reg = text[reg_start:reg_end]
lookup = text[reg_end:]
assert "hierarchy_id == NULL || !MrmHierarchyValid(hierarchy_id)" in reg
assert "names[ndx] == NULL" in reg
output_guard = lookup.index("if (value_return == NULL)")
clear_output = lookup.index("*value_return = NULL;", output_guard)
hierarchy_guard = lookup.index(
    "hierarchy_id == NULL || !MrmHierarchyValid(hierarchy_id)", clear_output)
name_guard = lookup.index("if (name == NULL)", hierarchy_guard)
fallback = lookup.index("Urm__WCI_LookupRegisteredName", name_guard)
if not output_guard < clear_output < hierarchy_guard < name_guard < fallback:
    raise SystemExit("hierarchy lookup does not clear output before failed lookup paths")
print("Mrm hierarchy name APIs reject invalid inputs and clear lookup output")
