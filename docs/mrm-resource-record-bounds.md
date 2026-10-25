# Mrm resource-descriptor bounds

Embedded `RGMResourceDesc` values now validate their declared size before field
access. RID descriptors must contain the fixed record, while indexed references
must contain a NUL-terminated key within the declared bytes. The same check
protects ordinary argument conversion and callback resource values; external
literal contexts continue through their existing reader path.

The source-level regression check is
`tests/automated/check-mrm-resource-record-bounds.py`.
