# XmString entry allocation failures

Internal unoptimization and segment insertion now allocate array entries with
an explicit failure check before initializing them. Failed entry allocation
leaves the source string and any copied segment ownership intact instead of
dereferencing a missing entry or leaking temporary storage.
