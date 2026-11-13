#!/usr/bin/env python3
"""Write the seed corpora of the targets that take hand-made input:
editres-wire, xmstring-stream and xm-converters, into corpus/NAME next to this
script.  The seeds of mrm-uid are written by the resource manager's own writer:
run-fuzz.py --make-seeds corpus/mrm-uid --scratch DIR.

    make-seeds.py
"""
from pathlib import Path
import struct

HERE = Path(__file__).resolve().parent


def write(target, name, data):
    d = HERE / "corpus" / target
    d.mkdir(parents=True, exist_ok=True)
    (d / name).write_bytes(bytes(data))


# ---- Editres wire protocol ----------------------------------------------------
# request: identifier, command, Card32 body length, body

def card16(v):
    return struct.pack(">H", v)


def string8(s):
    return card16(len(s)) + s.encode()


def path(*ids):
    return card16(len(ids)) + b"".join(struct.pack(">I", i) for i in ids)


def request(command, body, ident=1):
    return bytes([ident, command]) + struct.pack(">I", len(body)) + body


write("editres-wire", "send-widget-tree", request(0, b""))
write("editres-wire", "set-values",
      request(1, string8("label") + string8("String") + card16(5) + b"Hello" + card16(2) +
              path(0x1000, 0x1010) + path(0x1000)))
write("editres-wire", "find-child", request(4, path(0x1000, 0x1010, 0x1020) + struct.pack(">hh", 10, -5)))
write("editres-wire", "get-geometry", request(3, card16(2) + path(0x1000) + path(0x1000, 0x1010)))
write("editres-wire", "get-values", request(5, string8("background") + card16(1) + path(0x1000, 0x1010)))


# ---- compound string external format ------------------------------------------
# df 80 06, length (one byte below 128, else 0x82 and two bytes), components

TEXT, DIRECTION, SEPARATOR, LOCALE_TEXT, LOCALE, LAYOUT_PUSH, LAYOUT_POP, REND_BEGIN, REND_END, TAB = \
    2, 3, 4, 5, 6, 8, 9, 10, 11, 12
CHARSET = 1


def comp(tag, value=b""):
    n = len(value)
    return bytes([tag]) + (bytes([n]) if n < 128 else b"\x82" + card16(n)) + value


def stream(*components):
    body = b"".join(components)
    n = len(body)
    return b"\xdf\x80\x06" + (bytes([n]) if n < 128 else b"\x82" + card16(n)) + body


write("xmstring-stream", "locale-text", stream(comp(LOCALE_TEXT, b"Hello world")))
write("xmstring-stream", "tag-text-separator",
      stream(comp(CHARSET, b"tag1"), comp(TEXT, b"first line"), comp(SEPARATOR),
             comp(CHARSET, b"tag2"), comp(TEXT, b"second line")))
write("xmstring-stream", "direction-layout-tab",
      stream(comp(DIRECTION, b"\x01"), comp(LAYOUT_PUSH, b"\x00"), comp(TAB), comp(TEXT, b"cell"),
             comp(LAYOUT_POP, b"\x00"), comp(TEXT, b"x")))
write("xmstring-stream", "renditions",
      stream(comp(REND_BEGIN, b"bold"), comp(LOCALE, b"locale"), comp(TEXT, b"bold text"), comp(REND_END, b"bold")))
write("xmstring-stream", "long-form",
      stream(comp(TEXT, b"abcdefghij" * 20), comp(SEPARATOR), comp(TEXT, b"end")))


# ---- resource converters ------------------------------------------------------
# first byte: converter (0 tab list, 1 render table, 2 button render table, 3 font list, 4 key symbol table,
# 5 character set table, 6 string table of compound strings, 7 string table, 8 cardinal list, 9 atom list,
# 10 compound string, 11 key symbol, 12-15 positions and dimensions, 16 boolean dimension, 17 cardinal,
# 18 text position, 19 char), then the string

def conv(index, text):
    return bytes([index]) + text.encode()


write("xm-converters", "tablist", conv(0, "1.0in, 2cm, +0.5in, 30mm"))
write("xm-converters", "rendertable", conv(1, "tag1 = fixed, tag2 = variable, *"))
write("xm-converters", "fontlist", conv(3, "-*-fixed-medium-r-*-*-14-*=tag1, fixed=tag2"))
write("xm-converters", "keysym-table", conv(4, "Escape, F1, a, space, Return"))
write("xm-converters", "string-table", conv(6, "one,two\\,three,four"))
