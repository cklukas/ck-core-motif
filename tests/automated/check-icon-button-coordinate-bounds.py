#!/usr/bin/env python3
"""Check IconButton text coordinates cannot wrap through Position."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconButton.c").read_text(
    encoding="utf-8"
)
assert "#include <limits.h>" in source
assert "static Position bounded_position(long long value)" in source
assert "bounded_position((long long)XmIconButton_text_x(iw) + 1)" in source
assert "bounded_position((long long)XmIconButton_text_y(iw) + 1)" in source
assert "bounded_position((long long)XmIconButton_text_x(iw))," in source
assert "bounded_position((long long)XmIconButton_text_y(iw))," in source
assert "XmIconButton_pix_x(iw) = (Position)" not in source
assert "XmIconButton_pix_y(iw) = ((Position)" not in source
assert "XmIconButton_text_y(iw) = height - (V_SPACE(iw) + (Position)" not in source
print("PASS IconButton text coordinates stay bounded")
