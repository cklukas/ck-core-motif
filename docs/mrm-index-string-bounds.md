# Mrm index string bounds

Index entries store offsets into a fixed record string area. Search and resource
enumeration now use a shared terminator check that limits `strnlen` to both the
maximum index-name length and the bytes remaining in the record. Corrupt
near-end offsets therefore fail as bad records without reading beyond the
record buffer.
