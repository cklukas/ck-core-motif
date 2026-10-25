# Exm demonstration build portability

The Exm demonstration library is an educational example of Motif subclassing;
it is not part of the `libXm` ABI. Its compound-string widget is exposed as
`ExmString.h`.

The source tree previously carried a duplicate `String.h`. On a case-sensitive
filesystem that name appears distinct from the C standard `<string.h>`, but a
case-insensitive development filesystem exposes it as `string.h`. Because the
Exm source directory is on the compiler include path, X11's `Intrinsic.h`
could then include the Exm widget header recursively. The resulting diagnostics
reported missing `Widget`, `Arg`, `Cardinal` and `externalref` declarations.

The duplicate filename is removed, and the remaining legacy private header now
includes `ExmString.h`. This keeps the educational widget source unambiguous
without changing `libXm`, the Motif ABI, or the normal application headers.

The portability check is a clean out-of-tree build of the Exm target followed
by the full source-tree target. On 2026-10-25, the x86_64 cohort completed both
with `make -j4`; existing warnings from historical demonstration code remain
visible and are not suppressed.
