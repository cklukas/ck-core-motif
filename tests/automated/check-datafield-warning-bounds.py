#!/usr/bin/env python3
"""Check that DataField validation warnings cannot overflow a fixed buffer."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c"
text = source.read_text(encoding="utf-8")
assert "df_WarnInvalidChar" in text
assert "df_WarnInvalidString" in text
assert "snprintf(NULL, 0, MSG5" in text
assert "snprintf(NULL, 0, WC_MSG1" in text
assert "char warn_str[52]" not in text
assert "sprintf(warn_str, MSG5" not in text
assert "sprintf(warn_str, WC_MSG1" not in text
print("DataField warning formatting is dynamically bounded")
