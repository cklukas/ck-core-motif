# Native ABI cohort

CK Core Motif 2.4.0 uses native ABI epoch **6**. The suite's
libtool version is `6:0:0`: ELF installations provide `libXm.so.6`,
`libMrm.so.6` and `libUil.so.6`. These are rebuilt together. No alias to the
previous SONAME is provided.

Unicode input/layout and native semantic-provider changes affect private
records as well as adding public functions. In particular, the string source
record contains its cached Unicode index, text-field and text-output records
own layout/cache state, and the line-table width becomes an `int` so long
lines are not truncated to 16 bits. Applications which allocate subclasses,
embed these records, or access their members are binary consumers even though
the headers end in `P.h` or `I.h`.

The first published development cohort used epoch 5. Subsequent widget-owned
font/shaping context fields and the numeric accessibility value fields/callback
change native and public provider records again. Epoch 6 deliberately separates
those layouts; later record changes must not retain the earlier SONAME. Recompile accessibility providers as well as widget subclasses.

Every intended native consumer must therefore be rebuilt using the headers,
feature configuration and libraries from one exact installed source cohort.
This includes custom widget/subclass libraries, resource-compiler consumers,
Mrm/Uil integrations and applications. Reusing a previous object file or shared
widget library is not an ABI migration. Static linking also requires a clean
rebuild of affected consumers.

`XmVERSION`, `XmREVISION` and `XmUPDATE_LEVEL` identify source API version
2.4.0. `XmABI_VERSION` identifies native layout epoch 6;
`XmToolkitABI()` returns the linked library's epoch. Admission tests must compare
these values, prove the compilation header origin and inspect the executable's
loaded library path and SONAME. The numeric epoch alone does not prove that
optional features or exact source bytes match; retain the configured feature
manifest and source/library hashes as well.

Build into a fresh isolated prefix. Validate all required consumers there before
selecting that prefix. Never install a compatibility alias for an earlier SONAME, merge
old/new headers, or let runtime search paths select a library from another
prefix. Keep old admitted prefixes isolated while they are still in use, then
retire them after their consumers have moved to the verified replacement.
