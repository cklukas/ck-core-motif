# XmString component allocation failures

Cached tab and empty-string constructors now return `NULL` when their
representation allocation fails. Cloning and component-to-string conversion
also stop before copying into a missing representation, allowing callers to
release partial work and report failure safely.
