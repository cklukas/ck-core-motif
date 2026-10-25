# Xm image-construction failure handling

The shared `_XmCreateImage` macro now checks the `XCreateImage` result before
writing XImage fields. The built-in image cache also stops cleanly when its
shared XImage cannot be created, avoiding null dereferences in low-memory or
server-failure paths.

The source-level regression is `tests/automated/check-xm-create-image-null.py`.
