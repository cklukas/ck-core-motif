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


# ---- XPM image and metadata parser --------------------------------------------

write("xpm-image", "xpm3-comments-1cpp",
      b'/* XPM */\n/* hints */\nstatic char *i[]={\n"2 2 2 1 0 0",\n/* colors */\n'
      b'"a c #000000 m black",\n"b c #ffffff s background",\n/* pixels */\n"ab",\n"ba"};\n')
write("xpm-image", "xpm3-2cpp",
      b'/* XPM */\nstatic char *i[]={"2 2 2 2","aa c black","bb c white","aabb","bbaa"};\n')
write("xpm-image", "xpm3-extensions",
      b'/* XPM */\nstatic char *i[]={"2 2 2 1 XPMEXT","a c black","b c white","ab","ba",'
      b'"XPMEXT meta","line1","line2","XPMENDEXT"};\n')
write("xpm-image", "xpm2-plain",
      b"! XPM2\n! hints\n2 2 2 1\na c black\nb c white\nab\nba\n")
write("xpm-image", "xpm2-lisp",
      b'! XPM2 Lisp\n; hints\n(setq i \'("2 2 2 1""a c black""b c white""ab""ba"))\n')
write("xpm-image", "oversized-buffer-dims",
      b'* XPM "277777777 2 2 1"" c""bc')
write("xpm-image", "duplicate-color-key",
      b'/* XPM */\nstatic char *i[]={"2 2 2 1","a c #000000 c #111111","b c #ffffff","ab","ba"};\n')


# ---- MWM workspace wire protocol and .mwmrc parsers ---------------------------

write("mwm-parsers", "wsm-connect-req", bytes([0, 0, 3, 3, 2, 1]))
write("mwm-parsers", "wsm-extensions-req", bytes([1, 0, 1, 0, 3]) + b"abc")
write("mwm-parsers", "wsm-config-fmt-reply", bytes([2 | 0x80, 1, 0, 0, 0, 1, 0, 1]) + b"x" + bytes([32, 0, 0, 0]))
write("mwm-parsers", "wsm-set-state-req",
      bytes([4, 0, 1, 0, 0, 0, 1, 0xC0, 0, 0, 0, 5, 0, 2, 0, 8, 0, 9]))
write("mwm-parsers", "wsm-highbit-card32",
      bytes([0xFB, 0xFF, 0xFF, 0x0F, 0x20]))
write("mwm-parsers", "wsm-wm-windows-reply",
      bytes([8, 0, 1, 0, 0, 0, 0]))
write("mwm-parsers", "mwmrc-bindings",
      b'Shift Ctrl<Btn1Click> frame|title|icon f.Exec "xterm"\n'
      b'~Meta<Key>Return window|app f.circle_up icon|window\n'
      b'=<ClientCmd.SubCmd> _M Alt<Key>F4 f.cci cascade <Entry>\n')


# ---- UIL binary widget meta-language database (.wmd) --------------------------

def wmd_header(table_id, num_items, table_size):
    return struct.pack("iii", table_id, num_items, table_size)


wmd_globals = struct.pack("11i", 2, 4, 2, 2, 4, 2, 4, 2, 2, 16, 2)
wmd_constraint = wmd_header(1, 2, 2) + b"\x03\x01"
wmd_arg_types = wmd_header(5, 5, 5) + b"\x00\x01\x02\x03\x04"
wmd_child_class = wmd_header(29, 3, 3) + b"\x00\x01\x02"
wmd_charset_wr = wmd_header(7, 3, 3) + b"\x00\x01\x01"
wmd_charset_pa = wmd_header(8, 3, 3) + b"\x00\x01\x01"
wmd_charset_sz = wmd_header(9, 3, 3) + b"\x00\x01\x01"
wmd_lang_codes = wmd_header(11, 1, 4) + struct.pack("2H", 1, 2)
wmd_arg_enumset = wmd_header(14, 4, 10) + struct.pack("5H", 0, 1, 0, 2, 0)
wmd_related_arg = wmd_header(21, 4, 10) + struct.pack("5H", 0, 0, 0, 0, 0)
wmd_gadget_var = wmd_header(24, 4, 10) + struct.pack("5H", 0, 2, 0, 0, 0)
wmd_nondialog = wmd_header(25, 4, 10) + struct.pack("5H", 0, 1, 1, 0, 0)
wmd_subtree = wmd_header(26, 4, 10) + struct.pack("5H", 0, 0, 0, 0, 0)
wmd_enumvals = wmd_header(15, 4, 20) + struct.pack("5i", 0, 10, 20, 30, 40)
wmd_allowed_arg = wmd_header(4, 4, 5 * 8) + (b"\x0f" * 4)
wmd_widget_names = (
    wmd_header(16, 2, 3 * 8)
    + struct.pack("3i", 0, 13, 13)
    + b"user_defined\x00\x00"
    + b"XmPushButton\x00\x00"
)
wmd_key_entry = struct.pack("BHBBL", 1, 1, 5, 10, 0)
wmd_key_table = (
    wmd_header(2, 2, 2 * len(wmd_key_entry))
    + wmd_key_entry * 2
    + b"width\x00height\x00"
)
wmd_enum_entry_0 = struct.pack("h6xQ", 0, 0)
wmd_enum_entry_1 = struct.pack("h6xQ", 2, 0)
wmd_enum_set = (
    wmd_header(13, 2, 3 * len(wmd_enum_entry_0))
    + wmd_enum_entry_0 + wmd_enum_entry_1 + wmd_enum_entry_1
    + struct.pack("4H", 1, 2, 3, 4)
)

write("uil-wmd", "minimal-tables",
      wmd_globals + wmd_constraint + wmd_arg_types + wmd_child_class
      + wmd_charset_wr + wmd_charset_pa + wmd_charset_sz + wmd_lang_codes
      + wmd_arg_enumset + wmd_related_arg + wmd_gadget_var + wmd_nondialog
      + wmd_subtree + wmd_enumvals + wmd_allowed_arg + wmd_widget_names
      + wmd_key_table + wmd_enum_set)
write("uil-wmd", "strings-and-charsets",
      wmd_globals + wmd_widget_names
      + wmd_header(6, 2, 3 * 8) + struct.pack("3i", 0, 10, 6) + b"ISO8859-1\x00\x00UTF-8\x00\x00")
write("uil-wmd", "keys-case-ins",
      wmd_globals + wmd_key_table
      + wmd_header(3, 2, 2 * len(wmd_key_entry)) + wmd_key_entry * 2 + b"WIDTH\x00HEIGHT\x00")


# ---- Wayland X11r11 & XRender wire protocol server ----------------------------

def x11_req(opcode, data1, body=b""):
    pad = (-len(body)) & 3
    words = 1 + (len(body) + pad) // 4
    return struct.pack("<BBH", opcode, data1, words) + body + (b"\x00" * pad)


def x11_bigreq(opcode, data1, body=b""):
    pad = (-len(body)) & 3
    words = 2 + (len(body) + pad) // 4
    return struct.pack("<BBHI", opcode, data1, 0, words) + body + (b"\x00" * pad)


# 1) Full connection setup handshake + QueryExtension("RENDER") + InternAtom
write("wayland-x11-server", "handshake-and-atoms",
      struct.pack("<BxHHHHxx", ord("l"), 11, 0, 18, 16)
      + b"MIT-MAGIC-COOKIE-1\x00\x00" + (b"\xaa" * 16)
      + x11_req(98, 0, struct.pack("<HH", 6, 0) + b"RENDER")
      + x11_req(16, 0, struct.pack("<HH", 15, 0) + b"_MOTIF_WM_HINTS")
      + x11_req(17, 0, struct.pack("<I", 1)))

# 2) CreateWindow + ChangeProperty + GetProperty + ConfigureWindow + MapWindow
write("wayland-x11-server", "window-lifecycle-and-props",
      x11_req(1, 24, struct.pack("<IIhhHHHHII", 0x00200001, 0x40, 10, 20, 120, 80, 0, 1, 0x21, 0x0802, ) + struct.pack("<II", 0x112233, 0x20000))
      + x11_req(18, 0, struct.pack("<IIIBxxxI", 0x00200001, 39, 31, 8, 5) + b"Hello")
      + x11_req(20, 0, struct.pack("<IIIII", 0x00200001, 39, 0, 0, 64))
      + x11_req(12, 0, struct.pack("<IHxxIIII", 0x00200001, 0x000f, 4, 8, 140, 90))
      + x11_req(8, 0, struct.pack("<I", 0x00200001)))

# 3) CreatePixmap + CreateGC + PolyFillRectangle + PutImage + GetImage + CopyArea
write("wayland-x11-server", "pixmap-gc-drawing-and-image",
      x11_req(53, 24, struct.pack("<IIHH", 0x00200010, 0x40, 16, 16))
      + x11_req(55, 0, struct.pack("<III", 0x00200011, 0x00200010, 0x000c) + struct.pack("<II", 0xff3366, 0x102030))
      + x11_req(70, 0, struct.pack("<IIhhHH", 0x00200010, 0x00200011, 2, 2, 10, 10))
      + x11_req(72, 2, struct.pack("<IIHHhhBBxx", 0x00200010, 0x00200011, 2, 2, 0, 0, 0, 24) + (b"\x11\x22\x33\xff" * 4))
      + x11_req(73, 2, struct.pack("<IhhHHI", 0x00200010, 0, 0, 4, 4, 0xffffffff))
      + x11_req(62, 0, struct.pack("<IIIhhhhHH", 0x00200010, 0x00200010, 0x00200011, 0, 0, 4, 4, 8, 8)))

# 4) RENDER 0.11: QueryPictFormats + CreateSolidFill + CreatePicture + Composite + Trapezoids
write("wayland-x11-server", "xrender-composite-and-trapezoids",
      x11_req(53, 32, struct.pack("<IIHH", 0x00200020, 0x40, 32, 32))
      + x11_req(132, 1, b"")
      + x11_req(132, 33, struct.pack("<IHHHH", 0x00200021, 0xffff, 0x8000, 0x4000, 0xffff))
      + x11_req(132, 4, struct.pack("<IIII", 0x00200022, 0x00200020, 0x60, 0))
      + x11_req(132, 8, struct.pack("<BxxxIIIhhhhhhHH", 3, 0x00200021, 0, 0x00200022, 0, 0, 0, 0, 2, 2, 16, 16))
      + x11_req(132, 10, struct.pack("<BxxxIIIhh", 3, 0x00200021, 0x00200022, 0x62, 0, 0)
                + struct.pack("<10i", 0, 10 << 16, 0, 0, 0, 10 << 16, 10 << 16, 0, 10 << 16, 10 << 16)))

# 5) BIG-REQUESTS + RANDR + SHAPE + XFIXES + XINERAMA
write("wayland-x11-server", "extensions-bigreq-randr-shape-xfixes",
      x11_req(130, 0, b"")
      + x11_bigreq(18, 0, struct.pack("<IIIBxxxI", 0x40, 23, 31, 8, 12) + b"Xft.dpi:\t96\n")
      + x11_req(135, 0, struct.pack("<II", 1, 5))
      + x11_req(135, 8, struct.pack("<I", 0x40))
      + x11_req(133, 1, struct.pack("<BBxxIhh", 0, 0, 0x40, 0, 0) + struct.pack("<hhHH", 0, 0, 100, 100))
      + x11_req(134, 5, struct.pack("<I", 0x00200030) + struct.pack("<hhHH", 0, 0, 50, 50))
      + x11_req(136, 5, b""))


