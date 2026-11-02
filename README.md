# CK Core Motif

Development source now requires native ABI9: semantic appearance roles and
modern control geometry extend native widget records. Rebuild toolkit libraries and all consumers together; see the
[ABI policy](https://cklukas.github.io/ck-core-motif/abi.html).
Installed epoch9 qualification remains separate from historical cohort results
recorded in the validation documentation.

A maintained fork of the Motif native Xt widget toolkit, with corrections for
modern builds, selection handling, display scaling and input behavior.

## Modern C and macOS

The toolkit, WML generators, UIL compiler, `mwm` and modern widget gallery can
be built with current Clang in C23 mode. Native macOS builds use XQuartz's
X11/Xt libraries; Linux remains supported. Use GNU Make 4.3 or newer (`gmake`
on macOS). The port fixes obsolete declarations and callback types rather than
requiring an older C language mode, and supplies real Mach-O function/data
aliases for the existing deprecated widget names.

See the [build guide](docs/build.md#modern-c-and-macos) for dependencies,
configuration commands, typed callback migration and native regression tests.
The Mac gallery test installation includes the standalone demo applications and
their runtime resources. Modern dialog action rows use comfortable minimum
button widths, with compact density and natural-size opt-out support.
Gallery-launched demos inherit its selected theme, density and scale. The
[Airport demo](docs/airport-demo.md) provides two working airports with drag,
click and keyboard flight controls.
The [modern macOS UI audit](docs/modern-macos-ui-audit.md) covers gallery states,
Xft font selection, measured controls and theme-transition regressions.
Native build/runtime qualification and actual macOS desktop, clipboard,
input-method and accessibility qualification are separate. The
[Markdown changelog](CHANGES.md) describes this update; `CHANGES` retains the
full historical maintenance log.

## What the maintenance work improves

The maintenance work has one consistent aim: make the existing Motif API safer
to use without changing the public programming model. Fixes are small and
reviewable, but their combined effect is visible at three levels:

For people using Motif applications, the practical result is fewer crashes,
less corrupted UI state, and more reliable behavior when dialogs, lists,
selections, drag-and-drop transfers, Unicode text, images, or widgets are
partially initialized or receive malformed input. Valid legacy behavior and
public resource names remain unchanged.

For application programmers, resource and callback boundaries now validate
widget classes, output pointers, child initialization, sizes, positions, and
allocation results before touching toolkit state. Invalid requests fail
deterministically instead of producing undefined memory access, and each fix
has focused source-contract coverage plus a short maintenance note. Resource
updates also commit replacement strings, fonts, and render tables
transactionally, so allocation failure preserves the last usable state. The
result is easier debugging and safer integration across mixed widget cohorts.

For end users, these changes accumulate into a toolkit that remains familiar
while handling multilingual labels and text, high-resolution images, dynamic
selection workflows, accessibility paths, and modern display metrics more
predictably. Small fixes run their focused regression checks before commit;
the complete automated suite is reserved for critical changes and periodic
checkpoint validation. Published maintenance commits are mirrored to both
configured development remotes with matching history.

In short, programmers get deterministic failure handling and transactional
state updates; application users get fewer hangs, corrupt redraws and lost
input; and maintainers get focused regression evidence instead of relying on
manual inspection. Remaining platform and service-specific qualification gates
are called out explicitly in the validation status rather than being implied by
the presence of a successful build.

Recent hardening also covers Container outline and selection traversal: malformed
child, parent, constraint, or node state is rejected or skipped without
disturbing valid trees. This keeps ordinary expand/collapse and selection
callbacks compatible while making partially initialized widget hierarchies
safer for both applications and embedders.

Container drag-and-drop transfers now also stop cleanly when their destination
state has expired or is malformed, preventing a stale transfer from being
dereferenced during item movement.

The development 2.4.0 cohort uses native ABI epoch 9, including semantic appearance roles, modern control
records, Unicode text maps and optional native accessibility records. Rebuild every
native consumer against one consistent header/library cohort; this is not a
drop-in replacement for `libXm.so.4`.

The historical installed ABI8 cohort passed its selected native feature fixtures,
including native list identities, Selection, Unicode text maps, accessibility,
display-scale and allocation-failure recovery. External input-method,
application adoption and full cross-architecture validation remain separate
gates; system/Xt ownership remains outside the toolkit sanitizer scope.

Read the [documentation](https://cklukas.github.io/ck-core-motif/),
[build guide](https://cklukas.github.io/ck-core-motif/build.html),
[ABI policy](https://cklukas.github.io/ck-core-motif/abi.html) and
[validation status](https://cklukas.github.io/ck-core-motif/status.html).
Historical manuals remain under `doc/`. The ckdocs workflows publish the site
to GitHub Pages and the configured self-hosted Pages service.

See `COPYING` for licensing terms, `CHANGES` for earlier fork corrections and
`RELNOTES` for inherited release information.
