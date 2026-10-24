# Mrm fetched font-list bounds

`UrmFetchSetValues` translates the variable-sized font-list record used by
legacy (URM 1.1 and earlier) UID files before converting it into a widget
argument. The record count is signed input from the file. The translation now
rejects zero, negative, and too-large counts before calculating the allocation
size. Invalid values are released and reported as conversion failures, while
valid font lists retain their existing conversion behavior.

The source-level regression is
`tests/automated/check-mrm-fetch-fontlist-bounds.py`.
