from pathlib import Path
import os

source = Path(os.environ["MOTIF_SOURCE"])
text = source.read_text()
reg_start = text.index("Urm__RegisterNamesInHierarchy (MrmHierarchy")
reg_end = text.index("Urm__LookupNameInHierarchy (MrmHierarchy", reg_start)
reg = text[reg_start:reg_end]
lookup = text[reg_end:]
assert "hierarchy_id == NULL || !MrmHierarchyValid(hierarchy_id)" in reg
assert "names[ndx] == NULL" in reg
assert "name == NULL || value_return == NULL" in lookup
print("Mrm hierarchy name APIs reject invalid handles and null names")
