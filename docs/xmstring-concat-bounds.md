# XmString concatenation allocation bounds

`XmStringConcatAndFree` combines line tables from two non-optimized strings.
The implementation now rejects counts that would wrap an unsigned line-count
calculation or exceed the byte range accepted by Xt allocation APIs. On this
failure path it releases both input strings, unlocks the Motif process lock,
and returns `NULL` without partially modifying either input.

The table byte count is converted explicitly to an Xt `Cardinal`, making the
allocation width and its checked range visible at the call site.
