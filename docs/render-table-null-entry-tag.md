# Null tags in render-table entries

Normal rendition creation supplies a tag, but a failed or externally built
entry can still leave a render-table slot with a null tag.  Lookup and removal
now skip such entries before comparing tags.  Valid tagged entries retain the
same matching and removal behavior, while malformed entries no longer cause a
null-pointer dereference.
