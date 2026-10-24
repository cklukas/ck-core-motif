#!/usr/bin/env python3
"""Check Mrm hierarchy filename suffix handling validates short names."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmhier.c"
text = source.read_text(encoding="utf-8")
start = text.rindex("I18NOpenFile (Display")
end = text.index("\n  /*\n   * No .uid suffix", start)
region = text[start:end]
null_pos = region.index("if (name == NULL)")
length_pos = region.index("name_len = strlen(name)")
suffix_pos = region.index('strcmp (name + name_len - 4,".uid")')
if not (null_pos < length_pos < suffix_pos):
    raise SystemExit("Mrm filename validation is ordered incorrectly")
if "name_len < 4" not in region[suffix_pos - 40:suffix_pos + 80]:
    raise SystemExit("short Mrm filename guard is missing")
if "&name[strlen(name)-4]" in region:
    raise SystemExit("unsafe Mrm filename suffix expression remains")

print("Mrm hierarchy filename suffixes validate short and null names")
