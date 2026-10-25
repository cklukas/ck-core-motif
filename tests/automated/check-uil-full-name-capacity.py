from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSymDef.h").read_text()

assert source.count("full_file_name[256]") == 2
assert "full_file_name[255]" not in source
