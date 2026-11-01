#!/usr/bin/env python3
"""Check MWM rejects an invalid initial multibyte command line safely."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text(
    encoding="utf-8"
)
start = source.index("unsigned char *\nGetNextLine")
end = source.index("} /* END OF FUNCTION GetNextLine */", start)
region = source[start:end]

initial_check = region.index("chlen = mblen((char *)parseP, MB_CUR_MAX);")
guard = region.index("if (chlen < 0)", initial_check)
copy_loop = region.index("while ((*parseP != '\\0')", guard)

assert "string = NULL;" in region[guard:copy_loop]
assert "else\n#endif\n\t{\n\t    while" in region[guard:copy_loop + 40]
assert "if(chlen==-1) string = NULL;" not in region
assert "*(string++) = *(parseP++);" in region
print("PASS MWM invalid initial multibyte lines cannot enter the copy loop")
