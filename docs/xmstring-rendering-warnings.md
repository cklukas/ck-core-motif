# XmString rendering warning paths

The optimized-to-nonoptimized conversion and optimized line renderer use
stack-local optimized segment records. Generic entry setters also support a
larger unoptimized record, so using those setters on the stack-local record
caused compiler out-of-bounds diagnostics. The optimized fields are now
assigned directly.

The entry-copy size branches and allocation check now explicitly guard the
allocation, and
rendition-tag cleanup only reallocates an initialized size. When all tags are
removed, the old tag array is released explicitly. These changes preserve the
existing XmString and rendition APIs while making ownership and bounds
contracts explicit.
