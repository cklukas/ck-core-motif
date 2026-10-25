# XmString conversion allocation failures

Internal XmString conversion constructors now check representation allocation
before initializing headers or segments. Segment extraction and both ASN.1
conversion directions propagate `NULL` cleanly, allowing callers to release
partial work instead of dereferencing a failed representation.
