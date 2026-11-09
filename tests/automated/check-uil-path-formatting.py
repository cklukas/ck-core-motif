#!/usr/bin/env python3
"""Check that UIL WMD path formatting sizes its expanded result exactly."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilDB.c").read_text()
start = source.index("String init_wmd_path(String filename)")
end = source.index("\n}\n", start) + 2
region = source[start:end]

assert "if (filename == NULL)" in region
assert "if (homedir == NULL)" in region
assert "snprintf(NULL, 0, WMDPATH_DEFAULT" in region
assert "snprintf(NULL, 0, XAPPLRES_DEFAULT" in region
assert region.count("(size_t)path_length >= (size_t)UINT_MAX") == 2
assert "(size_t)path_length + 1" in region
assert region.count("if (wmd_path == NULL)") == 2
assert "sprintf( wmd_path" not in region
assert "sprintf(wmd_path" not in region

open_file = source[source.index("void db_open_file ()") : source.index("String get_root_dir_name()")]
assert "if (wmdPath == NULL)" in open_file
assert "if (resolvedname == 0)" in open_file
assert "XtFree(wmdPath);\n\t    return;" in open_file
assert "if (dbfile == NULL)" in open_file
assert "XtFree(resolvedname);\n\t    return;" in open_file

home = source[source.index("String get_root_dir_name()") : source.index("String init_wmd_path(String filename)")]
assert "uid_t uid;" in home
assert "int uid;" not in home
assert "_XGetpwuid(uid, pwd_buf)" in home
print("UIL WMD path expansion is sized and formatted safely")
