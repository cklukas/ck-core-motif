#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/NumberBox.c").read_text()
start = source.index("static void step_value(")
end = source.index("\nstatic void Events", start)
region = source[start:end]
assert "#include <stdint.h>" in source
assert "int64_t delta" in region
assert "int64_t next" in region
assert "(int64_t)number->number_box.value" in region
assert "long next" not in region
print("NumberBox widened arithmetic present")
