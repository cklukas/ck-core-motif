/* Validate the complete wire envelope before the legacy allocation decoder.
 * Counts, masks and scalar widths are checked without reading beyond length. */
#include "wsm_proto.h"
#include <stddef.h>
typedef struct { const unsigned char *data; size_t remaining; Boolean valid; } Wire;
static unsigned long number(Wire *wire, unsigned size)
{
    unsigned long value = 0;
    if (!wire->valid || size > wire->remaining) { wire->valid = False; return 0; }
    wire->remaining -= size;
    while (size--) value = (value << 8) | *wire->data++;
    return value;
}
static void skip(Wire *wire, size_t size)
{
    if (!wire->valid || size > wire->remaining) { wire->valid = False; return; }
    wire->data += size; wire->remaining -= size;
}
static void strings(Wire *wire)
{
    unsigned long count = number(wire, 2);
    while (count-- && wire->valid) { unsigned long size = number(wire, 2); skip(wire, size); }
}
static void values(Wire *wire, Display *display, int screen, WSMConfigFormatType type)
{
    WSMConfigFormatData *format = _WSMGetConfigFormat(display, screen, type);
    if (!format || format->num_attrs < 0 || (format->num_attrs && !format->attr_list)) {
        wire->valid = False; return;
    }
    size_t size = (size_t)format->num_attrs / 8 + 1;
    const unsigned char *mask = wire->data;
    skip(wire, size);
    if (!wire->valid) return;
    /* Reject unused bits rather than silently accepting unexplained payload. */
    for (size_t i = (size_t)format->num_attrs; i < size * 8; ++i)
        if (mask[i / 8] & (0x80 >> (i % 8))) wire->valid = False;
    for (int i = 0; i < format->num_attrs && wire->valid; ++i) {
        WSMAttribute *attribute = &format->attr_list[i];
        if (!(mask[i / 8] & (0x80 >> (i % 8)))) continue;
        if (attribute->size != 8 && attribute->size != 16 && attribute->size != 32) {
            wire->valid = False; return;
        }
        unsigned long count = attribute->is_list ? number(wire, 2) : 1;
        skip(wire, count * (attribute->size / 8));
    }
}
static void windows(Wire *wire, Display *display, int screen)
{
    unsigned long count = number(wire, 2);
    while (count-- && wire->valid) {
        Window window = number(wire, 4);
        values(wire, display, screen, _WSMGetConfigFormatType(window));
    }
}
Boolean _WSMValidateMessage(Display *display, int screen, MessageData data,
                           unsigned long length, WSMRequestType type, Boolean reply)
{
    Wire wire = {(const unsigned char *)data, length, True};
    if (!data && length) return False;
    switch (type) {
    case WSM_CONNECT:
        if (reply) (void)number(&wire, 1);
        else { unsigned long count = number(&wire, 2); skip(&wire, count); }
        break;
    case WSM_EXTENSIONS: strings(&wire); break;
    case WSM_CONFIG_FMT:
        if (reply) {
            (void)number(&wire, 1);
            for (int type = 0; type < 3 && wire.valid; ++type) {
                unsigned long count = number(&wire, 2);
                while (count-- && wire.valid) {
                    unsigned long size = number(&wire, 2); skip(&wire, size);
                    unsigned long bits = number(&wire, 1), list = number(&wire, 1);
                    if ((bits != 8 && bits != 16 && bits != 32) || list > 1) wire.valid = False;
                }
            }
        }
        break;
    case WSM_GET_STATE:
        if (reply) windows(&wire, display, screen);
        else skip(&wire, 5);
        break;
    case WSM_SET_STATE:
        if (!reply) windows(&wire, display, screen);
        break;
    case WSM_REG_WINDOW:
        if (reply) values(&wire, display, screen, WSM_WINDOW_FMT);
        else skip(&wire, 4);
        break;
    case WSM_WM_GET_BACKGROUND_WINDOW: skip(&wire, reply ? 4 : 2); break;
    case WSM_WM_SET_BACKGROUND_WINDOW: skip(&wire, 4); break;
    case WSM_WM_WINDOWS: {
        unsigned long count;
        if (!reply) {
            skip(&wire, 4);
            count = number(&wire, 2); skip(&wire, count * 4);
        }
        count = number(&wire, 2);
        while (count-- && wire.valid) {
            unsigned long entries = number(&wire, 2); skip(&wire, entries * (reply ? 4 : 8));
            if (reply) { entries = number(&wire, 2); skip(&wire, entries * 4); }
        }
        break;
    }
    case WSM_WM_FOCUS:
        if (reply) skip(&wire, 4);
        break;
    case WSM_WM_POINTER:
        if (reply) skip(&wire, 8);
        break;
    default: return False;
    }
    return wire.valid && wire.remaining == 0;
}
