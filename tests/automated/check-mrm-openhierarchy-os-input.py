from pathlib import Path
import os

source = Path(os.environ.get(
    "MOTIF_SOURCE", Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmhier.c"))
text = source.read_text()
start = text.index("Urm__OpenHierarchy (MrmCount")
end = text.index("#ifndef NO_MESSAGE_CATALOG", start)
block = text[start:end]
assert "os_ext_list != NULL && *os_ext_list == NULL" in block
assert block.index("os_ext_list != NULL && *os_ext_list == NULL") < block.index("return MrmFAILURE")
print("Mrm hierarchy open rejects null OS extension objects")
