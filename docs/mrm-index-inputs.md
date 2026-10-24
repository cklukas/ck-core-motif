# Mrm index traversal inputs

Mrm index search and B-tree traversal helpers now reject missing file handles,
search strings, record buffers, and output pointers before dereferencing them.
This preserves valid traversal while turning malformed internal calls into a
clean `MrmFAILURE`.

The source-level regression is `tests/automated/check-mrm-index-inputs.py`.
