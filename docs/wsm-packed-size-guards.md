---
nav_exclude: true
---

# WSM packed-size guards

The workspace-manager protocol computes message sizes before allocating a
packed request or reply.  Those calculations now reject negative data lengths
and use checked additions for request and reply window data, extension strings,
window entries, and nested arrays.
If a malformed or extreme count cannot be represented by the `int` sizing
contract, packing fails before allocating or writing a truncated buffer.  The
wire format and valid message handling are unchanged.
