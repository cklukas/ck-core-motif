# RCMenu dynamic-array bounds

Menu callback snapshots, exception-pane ancestry, and post-from widget lists
now check their counts before allocation growth. Unrepresentable menu state
returns safely or stops the ancestry walk; ordinary callback and posting
behavior is unchanged.

The source-level regression is `tests/automated/check-rcmenu-bounds.py`.
