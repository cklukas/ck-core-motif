#!/usr/bin/env python3
"""Check bounded PWD and relative-path handling in Xmos path helpers."""
from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "Xmos.c").read_text(encoding="utf-8")
start = text.index("GetCurrentDir")
end = text.index("String\n_XmOSInitPath", start)
region = text[start:end]
assert "strlen(pwd) < MAX_DIR_PATH_LEN" in region
assert "prefix_len = (size_t)(filePart - buf)" in region
assert "suffix_len >= MAX_DIR_PATH_LEN - prefix_len" in region
assert "suffix_len >= MAX_DIR_PATH_LEN - base_len" in region
print("Xmos path construction is bounded")
