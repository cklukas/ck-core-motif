# Mrm index collapse status

Index collapse helpers now return `Cardinal` status values.  Allocation and
record-validation failures are propagated by both leaf and node split paths,
which prevents a split from publishing partially rebuilt records.  Successful
collapses retain the existing serialized index layout and return
`MrmSUCCESS`.

The source-level regression is
`tests/automated/check-mrm-index-collapse-status.py`.
