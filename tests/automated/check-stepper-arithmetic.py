#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/Stepper.c").read_text()
start = source.index("static void step(")
end = source.index("\nstatic void Events", start)
region = source[start:end]
assert "#include <stdint.h>" in source
assert "int64_t next" in region
assert "(int64_t)s->stepper.value" in region
assert "-(int64_t)s->stepper.step" in region
assert "long next" not in region
print("Stepper widened arithmetic present")
