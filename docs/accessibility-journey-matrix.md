---
nav_exclude: true
---
# Accessibility journey and semantic coverage matrix

This matrix separates protocol-level evidence from user-observable assistive
technology behavior. A provider fixture is not a screen-reader pass by itself.

| Surface | Native/provider evidence | Real-client status | Open boundary |
| --- | --- | --- | --- |
| MenuBar, PulldownMenu, PopupMenu | Roles, names, expansion and activation fixtures pass on AArch64 | Orca announces the pulldown and item during open, keyboard navigation, Escape and Return | Application popup journeys and additional window managers. |
| List and ListItem | Identity, names, selection, stale-target refusal, actions and reentrancy pass | Orca announces the list and both rows during a native selection change. The selected child matches the native List position. | Additional desktop and application shells. |
| Text and TextField | UTF-8 value, caret, selection, preedit and exact text-event fixtures pass | Keyboard insertion, selection, replacement and deletion match exact AT-SPI offsets and payloads under Orca. Speech checks include the current line, selection and resulting title. | Speech echo for every edit command, IMEs and other screen readers. |
| Dialog and modality | Modal admission, focus/value/selection refusal, nested and destruction cases pass | Native input journey | Application-specific dialog announcements and WM behavior. |
| Disclosure | Role, expanded state, action and destruction fixtures pass | Focused provider journey | Broad assistive-client interaction. |
| ScrollBar | Value, orientation, range and action fixtures pass | Focused provider journey | Screen-reader presentation and continuous adjustment semantics. |

The current AArch64 installed cohort includes real D-Bus/AT-SPI observers and
a bounded Orca/eSpeak baseline. It does not claim x86_64, all desktop
screen-readers, candidate panels, actual application workflows, or clean
sanitizer exit ownership. The detailed provider and lifetime contracts remain
in `docs/accessibility.md`; aggregate admission limits remain in
`docs/status.md`.

## ABI 9 keyboard and speech journey

On Ubuntu 26.04 AArch64, the installed ABI 9 fixture runs with Orca 50,
Speech Dispatcher and eSpeak-NG on a private Xvfb display and AT-SPI bus.
The test activates a native menu item with Return, checks List selection with
an independent D-Bus client, and verifies the contents and precise edit events
of a native Text and TextField. The receipt includes the exact library and
fixture fingerprints, keyboard steps, event payloads, speech output and
synthesized PCM.

## 2026-10-25 native Orca/eSpeak receipt

The bounded native-chrome journey was rerun from a fresh installed prefix on
the AArch64 `uconsole` host with `--enable-atspi --enable-xft
--enable-unicode-text`.  The observer compiled `native_controls.c` against
that prefix and verified the fixture process mapped exactly its
`libXm.so.8.0.0`.  The 37.02-second receipt reports 23 AT-SPI cache objects,
nine direct text/selection protocol calls, 113 recorded utterances, and all
required native utterances
for buttons, toggles, text fields, list/list-items, menu/menu-items,
disclosure and the horizontal scrollbar.  It produced 2,436,594 bytes of
synthesized eSpeak PCM.  Orca, Speech Dispatcher, the fixture and Openbox all
exited cleanly, and the receipt contains the executable and library SHA-256
fingerprints.

This qualifies the selected native-control journey and transport lifetime on
that installed AArch64 configuration, including bounded list-item and menu-item
announcements.  It does not promote complete List/Menu semantics across
desktop/application shells, precise edit-event echo, candidate panels, a
complete application, human usability, x86_64, other screen readers/desktops
or sanitizer ownership to supported scope.

## 2026-10-25 focused semantic rerun

The fresh installed ABI-8 cohort also passed the strict native/provider
fixtures for accessibility, text events, List and Menu (`accessibility`,
`text-events`, `list-accessibility` and `menu-accessibility`) in 19.79 seconds.
The run used the pinned corrected Xt dependency directory recorded by the
native-feature receipt and kept the exact `libXm.so.8` prefix mapping. This
extends regression evidence for the existing semantics; it does not close the
desktop/application-shell or x86_64 boundaries in the matrix above.

When extending this matrix, add a native regression for a toolkit defect first,
then rerun the actual client journey. If a service or screen-reader is absent,
record it as unavailable instead of substituting a direct provider call.
