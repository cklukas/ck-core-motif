#!/usr/bin/env python3
from pathlib import Path
text = (Path(__file__).parents[2] / 'clients/uil/UilSrcSrc.c').read_text()
assert 'memcpy(c_buffer, src_az_current_source_buffer->c_text,' in text
assert 'src_k_max_source_line_length + 1' in text
print('UIL cached source copy is bounded')
