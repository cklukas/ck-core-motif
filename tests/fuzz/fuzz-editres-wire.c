/*
 * Fuzz target: the Editres wire protocol request parser (lib/Xm/EditresCom.c).
 *
 * run-fuzz.py cuts the type definitions, the byte reader (WireGet8, WireGet16,
 * WireGetSigned16, WireGet32, WireGetBytes, WireGetString8, WireGetWidgetPath,
 * WireGetPathList), BuildEvent and FreeEvent out of the production file into
 * production.h.  The input is a complete protocol request, exactly as the
 * resource editor sends it in the EditresProtocol selection:
 *
 *   byte 0      request identifier (the harness answers to whatever it is)
 *   byte 1      command (0 SendWidgetTree ... 5 GetValues)
 *   bytes 2-5   body length, most significant byte first
 *   body        command dependent: strings (Card16 length + bytes), widget
 *               paths (Card16 count + Card32 ids), Card16 and Int16 values
 *
 * The request is handed to BuildEvent in a heap block of exactly the received
 * length, so a read past the end is an address error.  The command handlers
 * (ExecuteCommand and what it calls) need a live widget tree on an X server and
 * are not part of this target.
 *
 * Stubbed: SendFailure (the reply to the editor) only counts calls.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include FUZZ_TYPES_H

static unsigned long failures_sent;

static void SendFailure(Widget w, Atom sel, ResIdent ident, const char *str)
{
    (void) w; (void) sel; (void) ident;
    failures_sent++;
    if (str) (void) strlen(str);
}

static void FreeEvent(EditresEvent *event);

#include FUZZ_PRODUCTION_H

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    uint8_t *copy = malloc(size ? size : 1);
    if (!copy) abort();
    if (size) memcpy(copy, data, size);
    ResIdent ident = size ? copy[0] : 0;
    EditresEvent *event = BuildEvent(NULL, 0, size ? copy : NULL, ident, size);
    if (event) {
        /* what the handlers read of a parsed request must be there */
        for (unsigned i = 0; i < event->num_paths; i++)
            for (unsigned j = 0; j < event->paths[i].count; j++)
                (void) event->paths[i].ids[j];
        if (event->name) (void) strlen(event->name);
        if (event->res_type) (void) strlen(event->res_type);
        if (event->value) (void) strlen(event->value);
        FreeEvent(event);
    }
    free(copy);
    return 0;
}
