# Xm argument filtering validation

`_XmFilterArgs` now initializes its returned list and count before processing,
rejects null output pointers, null argument arrays, and unrepresentable
allocation sizes, and treats a null filter list as no filtering. Valid argument
filtering remains unchanged. This prevents malformed private helper calls from
crashing or wrapping the temporary `Arg` allocation while retaining the Motif
ABI and existing valid-call behavior.
