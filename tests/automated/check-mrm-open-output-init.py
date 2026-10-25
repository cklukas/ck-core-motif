from pathlib import Path
import os

source = Path(os.environ["MOTIF_SOURCE"])
text = source.read_text()
for name, anchor in (("UrmIdbOpenFileRead (String", "Idb__FU_OpenFile"),
                     ("UrmIdbOpenBuffer (unsigned char", "Idb__HDR_GetHeader")):
    start = text.index(name)
    end = text.index("\n}\n", start)
    body = text[start:end]
    assert "*file_id_return = NULL;" in body
    assert body.index("*file_id_return = NULL;") < body.index(anchor)
print("Mrm open APIs clear output handles before attempting open")
