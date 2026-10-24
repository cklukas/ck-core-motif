# Accessibility journey and semantic coverage matrix

This matrix separates protocol-level evidence from user-observable assistive
technology behavior. A provider fixture is not a screen-reader pass by itself.

| Surface | Native/provider evidence | Real-client status | Open boundary |
| --- | --- | --- | --- |
| MenuBar, PulldownMenu, PopupMenu | Roles, names, expansion and activation fixtures pass on AArch64 | Focused Orca journey recorded | Complete menu navigation across desktop WMs and application shells. |
| List and ListItem | Identity, names, selection, stale-target refusal, actions and reentrancy pass | Protocol client coverage | Keyboard/Orca selection journey and complete list semantics. |
| Text and TextField | UTF-8 value, caret, selection, preedit and exact text-event fixtures pass | Focused native text observer | Actual screen-reader echo and complete editing notifications. |
| Dialog and modality | Modal admission, focus/value/selection refusal, nested and destruction cases pass | Native input journey | Application-specific dialog announcements and WM behavior. |
| Disclosure | Role, expanded state, action and destruction fixtures pass | Focused provider journey | Broad assistive-client interaction. |
| ScrollBar | Value, orientation, range and action fixtures pass | Focused provider journey | Screen-reader presentation and continuous adjustment semantics. |

The current AArch64 installed cohort includes real D-Bus/AT-SPI observers and
a bounded Orca/eSpeak baseline. It does not claim x86_64, all desktop
screen-readers, candidate panels, actual application workflows, or clean
sanitizer exit ownership. The detailed provider and lifetime contracts remain
in `docs/accessibility.md`; aggregate admission limits remain in
`docs/status.md`.

When extending this matrix, add a native regression for a toolkit defect first,
then rerun the actual client journey. If a service or screen-reader is absent,
record it as unavailable instead of substituting a direct provider call.
