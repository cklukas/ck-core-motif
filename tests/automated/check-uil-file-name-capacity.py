from pathlib import Path
import re


source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSymDef.h").read_text()

assert len(re.findall(r"\bfile_name\[256\]", source)) == 2
assert "file_name[255]" not in source
