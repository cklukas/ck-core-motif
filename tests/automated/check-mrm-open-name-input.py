from pathlib import Path
import os

source = Path(os.environ.get(
    "MOTIF_SOURCE", Path(__file__).parents[2] / "lib" / "Mrm" / "MrmItop.c"))
text = source.read_text()
start = text.index("UrmIdbOpenFileRead (String")
end = text.index("UrmIdbOpenBuffer (unsigned char", start)
body = text[start:end]
assert "name == NULL || file_id_return == NULL" in body
assert body.index("name == NULL || file_id_return == NULL") < body.index("Idb__FU_OpenFile")
print("Mrm file-open read rejects a null filename")
