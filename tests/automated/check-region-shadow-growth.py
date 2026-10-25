#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/Region.c").read_text()
assert "#include <stdint.h>" in source
assert source.count("XSegment *new_segml;") == 4
assert source.count("size_t new_count;") == 4
assert source.count("SIZE_MAX / (2 * sizeof(XSegment))") == 4
assert source.count("(size_t)UINT_MAX / (2 * sizeof(XSegment))") == 4
assert source.count("new_segml = (XSegment *) XtRealloc") == 4
assert source.count("if (new_segml == NULL)") == 4
assert source.count("*segml = new_segml;") == 4
assert source.count("*segmc = (int)new_count;") == 4
assert source.count("*segmc = *segmi;") == 8
assert "*segmi = *segmc = 0;" not in source
print("Region shadow segment growth guards present")
