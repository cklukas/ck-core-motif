# Hash-table resize chain linking

Hash-table resize now always records the current bucket predecessor when an
entry remains in its bucket. Previously that update was hidden behind the
optional `FIX_1264` macro; normal builds could therefore unlink the bucket
head when a later entry moved during rehash, losing entries from the table.

The focused regression is `tests/automated/check-xm-hash-resize-linking.py`.
