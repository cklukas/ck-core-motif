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

The public packer uses a `long` byte-count accumulator because it returns the
length through an `unsigned long` out parameter.  The internal per-window
helpers retain their `int` result contract, but now accumulate through the
same checked `long` helper before their proven-safe result conversion.  This
keeps the x86_64 build type-correct and avoids taking the address of a
`register` variable.  The focused check compiles and executes the production
checked-add helper under strict pointer-type warnings.
