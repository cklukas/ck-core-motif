from pathlib import Path
import os

source = Path(os.environ["MOTIF_SOURCE"])
text = source.read_text()
start = text.index("UrmHGetIndexedResource (MrmHierarchy")
end = text.index("Urm__RegisterNamesInHierarchy (MrmHierarchy", start)
body = text[start:end]
assert "if ( file_id_return == NULL )" in body
assert body.index("if ( file_id_return == NULL )") < body.index("*file_id_return =")
print("Mrm hierarchy resource lookup validates its output pointer")
