# Working menus in Periodic

Periodic's menus operate on the examples in the application. They are available
in classic and modern appearances and at the gallery's inherited display scale.
The Text example and the larger document window share one document.

| Menu | Behavior |
| --- | --- |
| File → New / Open | Create or load a text document; prompt before abandoning unsaved text. |
| File → Save / Save As | Write the document to disk. Save As confirms replacement. Errors preserve the document and existing file. |
| File → Print | Preview the document, then explicitly submit through a CUPS-compatible `lp` command. An optional printer name is passed as one argument. |
| File → Exit | Check the document and message draft for unsaved changes. Cancel, including Escape, retains the text. |
| Edit and the popup menu | Undo, Cut, Copy, Paste, Clear and Delete operate on the last active document or template editor. Clipboard operations use the toolkit's clipboard APIs. |
| View → Stack / Tile | Arrange open sample windows. Several Notebook → Dialogs examples can remain open together. |
| View → Day / Week / Month / Year | Open the corresponding calendar range, using the current local civil date. Week begins on Monday. |
| Options → Font | Select Small, Medium or Large without changing the theme or display scale. |
| Options → Direction | Change the last focused or activated arrow. The default target is the top arrow. |
| Options → Case Sensitive | Change matching in the document window's Find field. Unicode character positions are preserved. |
| Options → Word Wrap | Change line wrapping in both document views and the template. Cursor movement scrolls long unwrapped lines horizontally. |
| Help → On Context | Click an example for guidance, or press Escape to cancel. |
| Help → Window / Tutorial / Version | Open application help, a navigable five-step tutorial, or the toolkit version and font backend. |
| Template → Preview Message | Show the current message locally. No mail is sent. |
| Template → Include / Save Draft / Check English Spelling | Insert a text file, save the draft, or check English words against a system dictionary. The extra dialog buttons use the same actions. |
| File sample → File filter | Filter Audio (`*.wav`), Graphics (`*.png`) or Text (`*.txt`). Open previews text or binary-file metadata. Merge inserts selected text into the shared document; Clear selection clears the filename. |
| Weekday option menu | Select a weekday; its displayed label and menu history update together. |

Open accepts regular text files up to 4 MiB in the current locale; embedded NUL
bytes and invalid character sequences are rejected. Saves use a temporary sibling
file followed by a rename, preserving the permissions of existing files. Undo
retains up to 32 edits per document. Template Close retains its draft in memory;
Exit checks that draft too.

English spelling uses `/usr/share/dict/words` or
`/usr/share/dict/american-english`. Set `PERIODIC_DICTIONARY` to another word list
with one word per line. Non-English tokens are skipped. An unavailable dictionary
produces an explanatory error.

Print submission is asynchronous. The preview reports acceptance or an error
from `lp`. Cancel submission stops the submitting process; a job already accepted
by the print service may still need removal from its queue. Message Preview and
Save Draft are entirely local and do not require a print or mail service.

## Toolkit corrections found by this audit

Modern arrow painting now restores the complete rounded focus ring after direct
arm, release and pointer enter/leave painting. Previously, repainting the inner
rectangle could erase the curved corners and leave four disconnected segments.

The clipboard no longer frees records already consumed by property replacement.
Its record validation permits the retained previous item required by clipboard
Undo. Programmatic Text and TextField Paste calls can legitimately have no source
event; only secondary-selection insertion requires one. Menu dismissal preserves
the activated child so quick option-menu selections update the chosen item.
Root-shell traversal also uses window-to-root coordinates safely.

## Native regression

With demos built, run:

```sh
python3 tests/automated/run-periodic-menu-actions.py \
  --build /absolute/build --out /absolute/fresh-results
```

On a Mac, select results beneath a mounted `/Volumes/S8T/tmp` or
`/Volumes/PRO-BLADE/tmp`, and set `TMPDIR` there before invoking tools. The runner
uses an isolated Xvfb display and a fake `lp`, never the user's desktop or a real
printer. It inventories 45 terminal menu callbacks plus seven built-in weekday
choices, exercises the extra dialog buttons, and verifies real text/file bytes,
clipboard round trips, search counts, rendered wrap positions, arrow directions,
calendar pages, window positions, help navigation and cancellation. It runs light,
dark, 1.5× and classic cases. Font metrics/layout and all 192 modern arrow
combinations have separate native regressions. The
[native acceptance record](validation/periodic-menu-actions-20261102.json) identifies
the qualified source, binaries and results.
