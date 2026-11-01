#!/usr/bin/env python3
"""Check that UIL include-path output is copied into a bounded field."""
from pathlib import Path

source = Path('clients/uil/UilSrcSrc.c').read_text()
include = Path('clients/uil/UilSarInc.c').read_text()
header = Path('clients/uil/UilDefI.h').read_text()

assert 'src_copy_file_name(char *destination, size_t destination_size,' in source
assert 'if (length >= destination_size)' in source
assert 'length = destination_size - 1;' in source
assert 'src_copy_file_name(full_file_name, full_file_name_size,' in source
assert 'size_t full_file_name_size' in header
assert 'src_open_file (buffer, include_entry->full_file_name,' in include
assert 'sizeof(include_entry->full_file_name)' in include

print('UIL include filename output uses its destination capacity')
