from pathlib import Path
import os

root = Path(__file__).parents[2]
source = Path(os.environ.get("MOTIF_SOURCE", root / "lib" / "Mrm" / "MrmItop.c"))
text = source.read_text()
for name, anchor in (("UrmIdbOpenFileRead (String", "Idb__FU_OpenFile"),
                     ("UrmIdbOpenBuffer (unsigned char", "Idb__HDR_GetHeader")):
    start = text.index(name)
    end = text.index("\n}\n", start)
    body = text[start:end]
    assert "*file_id_return = NULL;" in body
    assert body.index("*file_id_return = NULL;") < body.index(anchor)

low_level = (root / "lib" / "Mrm" / "MrmIfile.c").read_text()
start = low_level.index("Idb__FU_OpenFile (char")
end = low_level.index("\n}\n", start)
body = low_level[start:end]
assert "if (name == NULL || file_id == NULL)" in body
assert "*file_id = NULL;" in body
assert body.index("*file_id = NULL;") < body.index("length = strlen (name);")

print("Mrm open APIs clear output handles before attempting open")
