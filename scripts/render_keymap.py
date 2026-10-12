#!/usr/bin/env python3
"""Render EC Type-K's satt keymap from QMK's physical geometry and keymap.c.

Run from any directory:
    python3 scripts/render_keymap.py
    python3 scripts/render_keymap.py --qmk-dir /path/to/qmk_firmware

Uses only the Python standard library. If rsvg-convert is installed, it also
renders an A4 PDF and PNG previews in docs/keymaps (or --output-dir).
SVGs and the parsed JSON remain available without it.
The source parser accepts LAYOUT arguments with nested calls, not arbitrary C.
Unknown keycodes, mismatched geometry, and unsupported layers fail explicitly.
"""

from __future__ import annotations

import argparse
from collections import Counter
from html import escape
import json
from pathlib import Path
import re
import shutil
import subprocess


ROOT = Path(__file__).resolve().parents[1]
UNIT = 84
GAP = 6
MARGIN = 46
TOP = 165
PANEL_HEIGHT = 690
WIDTH = 1700
FONT = "Hiragino Sans, Noto Sans CJK JP, sans-serif"

COLORS = {
    "regular": ("#ffffff", "#cbd5df", "#142536"),
    "modifier": ("#eaf0f5", "#bac8d4", "#213b51"),
    "changed": ("#e1effd", "#8ab7e1", "#124e80"),
    "inherit": ("#f0f3f6", "#d6dfe6", "#627487"),
    "trigger": ("#fff0cc", "#d4a949", "#674510"),
    "disabled": ("#e8ecef", "#d4dce2", "#81909a"),
    "danger": ("#ffe6e2", "#dc9a91", "#99382b"),
}

# Display names, not key assignments. Assignments always come from keymap.c.
SPECIAL = {
    "GUI_E": ("GUI+E",), "GUI_R": ("GUI+R",),
    "TAB_CTL": ("Tab", "長押し Ctrl"),
    "LTENT": ("Enter", "長押し L1"),
    "CTALDEL": ("Ctrl+Alt", "Delete"),
    "ZKHK": ("日本語", "切り替え"),
    "MO(2)": ("設定 L2", "押している間"),
    "CTLPGUP": ("Ctrl", "+PgUp"), "CTLPGDN": ("Ctrl", "+PgDn"),
    "ALTPGUP": ("Alt", "+PgUp"), "ALTPGDN": ("Alt", "+PgDn"),
    "QK_BOOT": ("書き込み", "モード"),
    "UG_TOGG": ("RGB", "ON/OFF"),
    "UG_NEXT": ("RGB", "次の効果"), "UG_PREV": ("RGB", "前の効果"),
    "KO_ON": ("記号補正", "ON"), "KO_OFF": ("記号補正", "OFF"),
    "KO_STAT": ("記号補正", "状態確認"),
}

BASIC = {
    "KC_ESC": "Esc", "KC_DEL": "Delete", "KC_BSPC": "Backspace",
    "KC_PGUP": "PgUp", "KC_PGDN": "PgDn", "KC_HOME": "Home", "KC_END": "End",
    "KC_LSFT": "Shift", "KC_RSFT": "Shift", "KC_LGUI": "GUI",
    "KC_LALT": "Alt", "KC_RCTL": "Ctrl", "KC_SPC": "Space",
    "KC_LEFT": "←", "KC_DOWN": "↓", "KC_UP": "↑", "KC_RGHT": "→",
    "XXXXXXX": "無効", "KC_NO": "無効",
}

SYMBOLS = {
    "KC_GRV": ("`", "~"), "KC_1": ("1", "!"), "KC_2": ("2", "@"),
    "KC_3": ("3", "#"), "KC_4": ("4", "$"), "KC_5": ("5", "%"),
    "KC_6": ("6", "^"), "KC_7": ("7", "&"), "KC_8": ("8", "*"),
    "KC_9": ("9", "("), "KC_0": ("0", ")"), "KC_MINS": ("-", "_"),
    "KC_EQL": ("=", "+"), "KC_BSLS": ("\\", "|"),
    "KC_LBRC": ("[", "{"), "KC_RBRC": ("]", "}"),
    "KC_SCLN": (";", ":"), "KC_QUOT": ("'", '"'),
    "KC_COMM": (",", "<"), "KC_DOT": (".", ">"), "KC_SLSH": ("/", "?"),
}

LAYER_INFO = {
    0: ("通常入力", "レイヤー 0", "普段の文字入力。小さい親指キーも、大きいキーと同じ機能です。"),
    1: ("数字・移動・Fキー", "レイヤー 1", "黄色の Enter キーを、どちらか1つ押し続けると有効。離すと通常入力へ戻ります。"),
    2: ("設定・書き込み", "レイヤー 2", "黄色の右下キーを押し続けると有効。離すと通常入力へ戻ります。"),
}


def without_comments(text: str) -> str:
    return re.sub(r"/\*.*?\*/|//[^\n]*", "", text, flags=re.S)


def layout_arguments(text: str, opening: int) -> tuple[list[str], int]:
    depth = 1
    start = opening + 1
    args = []
    for i in range(start, len(text)):
        char = text[i]
        if char == "(":
            depth += 1
        elif char == ")":
            depth -= 1
            if depth == 0:
                args.append(text[start:i].strip())
                return args, i + 1
        elif char == "," and depth == 1:
            args.append(text[start:i].strip())
            start = i + 1
    raise ValueError("Unclosed LAYOUT() invocation")


def parse_keymaps(source: str) -> dict[int, list[str]]:
    source = without_comments(source)
    layers = {}
    for match in re.finditer(r"\[(\d+)\]\s*=\s*LAYOUT\s*\(", source):
        layer = int(match[1])
        if layer in layers:
            raise ValueError(f"Duplicate layer {layer}")
        args, _ = layout_arguments(source, match.end() - 1)
        layers[layer] = [re.sub(r"\s+", "", arg) for arg in args]
    if set(layers) != {0, 1, 2}:
        raise ValueError(f"Expected layers 0, 1, 2; found {sorted(layers)}")
    return layers


def describe(code: str) -> tuple[tuple[str, ...], str | None]:
    if code in SPECIAL:
        return SPECIAL[code], None
    if code in SYMBOLS:
        plain, shifted = SYMBOLS[code]
        return (plain,), shifted
    if code in BASIC:
        return (BASIC[code],), None
    if re.fullmatch(r"KC_[A-Z]", code):
        return (code[3:],), None
    if re.fullmatch(r"KC_F(?:[1-9]|1[0-2])", code):
        return (code[3:],), None
    raise ValueError(f"Missing display label for {code!r}")


def txt(x: float, y: float, value: str, size=20, fill="#142536", weight=400, anchor="start") -> str:
    return (f'<text x="{x:g}" y="{y:g}" text-anchor="{anchor}" '
            f'font-size="{size}" font-weight="{weight}" fill="{fill}">{escape(value)}</text>')


def key_svg(position: dict, code: str, inherited: bool, layer: int, index: int, print_mode=False) -> str:
    x, y = MARGIN + position["x"] * UNIT, TOP + position["y"] * UNIT
    w, h = position.get("w", 1) * UNIT - GAP, position.get("h", 1) * UNIT - GAP
    active_trigger = (layer == 1 and code == "LTENT") or (layer == 2 and code == "MO(2)")
    if code in {"KC_NO", "XXXXXXX"}:
        category = "disabled"
    elif active_trigger or (layer == 0 and code in {"LTENT", "MO(2)"}):
        category = "trigger"
    elif code == "QK_BOOT":
        category = "danger"
    elif inherited:
        category = "inherit"
    elif layer:
        category = "changed"
    elif code in SPECIAL or code in {"KC_LSFT", "KC_RSFT", "KC_LGUI", "KC_LALT", "KC_RCTL"}:
        category = "modifier"
    else:
        category = "regular"
    fill, stroke, fg = COLORS[category]
    lines, shifted = describe(code)
    if print_mode:
        short_names = {
            "CTALDEL": ("Ctrl", "Alt", "Del"),
            "ZKHK": ("日本語", "切替"),
            "LTENT": ("Enter", "保持 L1"),
            "MO(2)": ("設定 L2", "保持"),
            "TAB_CTL": ("Tab", "保持 Ctrl"),
            "QK_BOOT": ("書込", "モード"),
            "KO_ON": ("補正", "ON"), "KO_OFF": ("補正", "OFF"),
            "KO_STAT": ("補正", "状態確認"),
        }
        lines = short_names.get(code, lines)
    tooltip = f"{code}; matrix {position['matrix']}; " + ("通常レイヤーから継承" if inherited else "このレイヤーの割り当て")
    output = [f'<g id="layer-{layer}-key-{index}" data-code="{escape(code)}" data-category="{category}">',
              f"<title>{escape(tooltip)}</title>",
              f'<rect x="{x:g}" y="{y + 3:g}" width="{w:g}" height="{h:g}" rx="9" fill="#dfe5eb"/>',
              f'<rect x="{x:g}" y="{y:g}" width="{w:g}" height="{h:g}" rx="9" fill="{fill}" stroke="{stroke}" stroke-width="{2 if active_trigger else 1}"/>']
    if shifted:
        output.append(txt(x + w / 2, y + 25, shifted, 18, fg, anchor="middle"))
        output.append(txt(x + w / 2, y + 59, lines[0], 28, fg, 500, "middle"))
    elif len(lines) == 3:
        for row, line in enumerate(lines):
            output.append(txt(x + w / 2, y + 23 + row * 22, line, 20, fg, 500, "middle"))
    elif len(lines) == 2:
        main_size = (20 if w < 90 else 25) if print_mode else (16 if w < 90 else 23)
        detail_size = (18 if w < 90 else 21) if print_mode else (12 if w < 90 else 16)
        output.append(txt(x + w / 2, y + 33, lines[0], main_size, fg, 500, "middle"))
        output.append(txt(x + w / 2, y + 58, lines[1], detail_size, fg, anchor="middle"))
    else:
        size = (20 if print_mode else 17) if len(lines[0]) >= 5 and w < 90 else 25
        output.append(txt(x + w / 2, y + 48, lines[0], size, fg, 500, "middle"))
    output.append("</g>")
    return "\n".join(output)


def panel(layer: int, geometry: list[dict], layers: dict[int, list[str]], print_mode=False) -> str:
    heading, tag, subtitle = LAYER_INFO[layer]
    parts = [f'<rect width="{WIDTH}" height="{PANEL_HEIGHT}" fill="#f8fafc"/>',
             txt(MARGIN, 47, "EC TYPE-K  /  satt", 19, "#536b80", 500),
             txt(MARGIN, 91, heading, 34, weight=500),
             txt(WIDTH - MARGIN, 86, tag, 22, "#536b80", 500, "end"),
             txt(MARGIN, 127, subtitle, 19, "#536b80")]
    for i, position in enumerate(geometry):
        code = layers[layer][i]
        inherited = code in {"_______", "KC_TRNS"}
        effective = layers[0][i] if inherited else code
        parts.append(key_svg(position, effective, inherited, layer, i, print_mode))
    legend_y = 622
    legends = [("trigger", "レイヤーを開くキー"), ("modifier", "特殊操作・修飾キー")] if layer == 0 else [
        ("changed", "このレイヤーで変わるキー"), ("inherit", "灰色：通常入力と同じ"),
        ("trigger", "押し続けるキー")]
    for j, (category, label) in enumerate(legends):
        x = MARGIN + j * 395
        parts.append(f'<rect x="{x}" y="{legend_y - 15}" width="19" height="19" rx="4" fill="{COLORS[category][0]}" stroke="{COLORS[category][1]}"/>')
        parts.append(txt(x + 29, legend_y, label, 21 if print_mode else 18, "#536b80"))
    notes = {
        0: "上段は Shift 時の記号。GUI＝MacのCommand／Windowsキー／LinuxのSuper。Alt＝MacのOption。",
        1: "「無効」は何も送信しません。Tab/Ctrl、Enter/L1 は、タップと長押しで動作が変わります。",
        2: "記号補正：JIS設定ならON、US設定ならOFF。状態確認は入力欄に ko-on / ko-off を出力。書き込みモードは通常入力が停止。",
    }
    if print_mode and layer == 2:
        notes[2] = "補正：JISならON、USならOFF。状態確認は ko-on / ko-off を入力。書込モードでは通常入力が停止。"
    parts.append(txt(MARGIN, 661, notes[layer], 21 if print_mode else 16, "#536b80"))
    return "\n".join(parts)


def svg_document(content: str, height: int, title: str) -> str:
    return (f'<svg xmlns="http://www.w3.org/2000/svg" width="{WIDTH}" height="{height}" '
            f'viewBox="0 0 {WIDTH} {height}" role="img" aria-labelledby="title desc">\n'
            f'<title id="title">{escape(title)}</title>\n'
            '<desc id="desc">QMKの物理キー座標と実際のキーマップから生成。灰色のキーは通常レイヤーの動作を継承。</desc>\n'
            f'<g font-family="{FONT}">\n{content}\n</g>\n</svg>\n')


def a4_document(geometry: list[dict], layers: dict[int, list[str]]) -> str:
    height = WIDTH * 297 / 210
    scale = 0.90
    parts = [f'<rect width="{WIDTH}" height="{height:g}" fill="#ffffff"/>',
             txt(100, 85, "EC Type-K / satt", 38, weight=500),
             txt(WIDTH - 100, 82, "全3レイヤー  |  A4・1枚", 23, "#536b80", anchor="end")]
    for layer in layers:
        offset = 125 + layer * PANEL_HEIGHT * scale
        parts.append(f'<g transform="translate(85,{offset:g}) scale({scale})">{panel(layer, geometry, layers, True)}</g>')
    notes = [
        "「保持」は、そのキーを押している間の動作。タップは短く押して離す操作です。",
        "日本語切替：Macは Ctrl+Space。Windowsは補正ONで半角/全角、OFFで Alt+`。LinuxはIME設定に依存。",
        "記号の表示はUS設定、またはJIS設定＋補正ONを前提。\\ の位置は環境により ¥ を入力します。",
        "各図は単独レイヤー表示。L1とL2を同時に保持した場合、透明キーは L2 → L1 → 通常の順で継承します。",
        f"キーマップのC定義と、QMK公式の物理キー座標からスクリプトで生成。{len(geometry)}キー × {len(layers)}レイヤー。",
    ]
    for i, note in enumerate(notes):
        parts.append(txt(100, 2070 + 48 * i, note, 23, "#425a70"))
    return (f'<svg xmlns="http://www.w3.org/2000/svg" width="210mm" height="297mm" '
            f'viewBox="0 0 {WIDTH} {height:g}" role="img" aria-labelledby="title">\n'
            '<title id="title">EC Type-K satt 全3レイヤー A4印刷用</title>\n'
            f'<g font-family="{FONT}">\n' + "\n".join(parts) + "\n</g>\n</svg>\n")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--qmk-dir", type=Path, default=ROOT.parent / "qmk_firmware")
    parser.add_argument("--output-dir", type=Path, default=ROOT / "docs" / "keymaps")
    parser.add_argument("--no-png", action="store_true")
    args = parser.parse_args()
    geometry_path = args.qmk_dir / "keyboards/cipulot/ec_typek/keyboard.json"
    source_path = ROOT / "keyboards/cipulot/ec_typek/keymaps/satt/keymap.c"
    geometry = json.loads(geometry_path.read_text())["layouts"]["LAYOUT"]["layout"]
    layers = parse_keymaps(source_path.read_text())
    if len({tuple(p["matrix"]) for p in geometry}) != len(geometry):
        raise ValueError("Duplicate matrix positions in QMK geometry")
    for layer, codes in layers.items():
        if len(codes) != len(geometry):
            raise ValueError(f"Layer {layer}: {len(codes)} keycodes but {len(geometry)} physical keys")
        for code in codes:
            if code not in {"_______", "KC_TRNS"}:
                describe(code)
    if any(code in {"_______", "KC_TRNS"} for code in layers[0]):
        raise ValueError("Transparent keys in base layer require an explicit fallback")
    if max(p["x"] + p.get("w", 1) for p in geometry) * UNIT + 2 * MARGIN > WIDTH:
        raise ValueError("Physical geometry exceeds drawing width")
    args.output_dir.mkdir(parents=True, exist_ok=True)
    sheets = {f"ec-typek-satt-layer-{layer}": svg_document(panel(layer, geometry, layers), PANEL_HEIGHT, f"EC Type-K satt レイヤー{layer}") for layer in layers}
    sheets["ec-typek-satt-all"] = svg_document(
        "\n".join(f'<g transform="translate(0,{layer * PANEL_HEIGHT})">{panel(layer, geometry, layers)}</g>' for layer in layers),
        PANEL_HEIGHT * len(layers), "EC Type-K satt 全3レイヤー")
    sheets["ec-typek-satt-a4"] = a4_document(geometry, layers)
    converter = shutil.which("rsvg-convert") if not args.no_png else None
    for name, svg in sheets.items():
        target = args.output_dir / f"{name}.svg"
        target.write_text(svg, encoding="utf-8")
        if converter:
            subprocess.run([converter, str(target), "-o", str(target.with_suffix(".png"))], check=True)
        print(target)
    pdf_converter = shutil.which("rsvg-convert")
    if pdf_converter:
        pdf_target = args.output_dir / "ec-typek-satt-a4.pdf"
        subprocess.run([pdf_converter, "--format", "pdf", "--page-width", "210mm", "--page-height", "297mm",
                        str(args.output_dir / "ec-typek-satt-a4.svg"), "-o", str(pdf_target)], check=True)
        print(pdf_target)
    else:
        print("rsvg-convert not found: PDF and PNG output skipped; install librsvg to render them.")
    metadata = {"keyboard": "cipulot/ec_typek", "keymap": "satt", "keys_per_layer": len(geometry), "layers": []}
    for layer, codes in layers.items():
        entries = []
        for i, (position, code) in enumerate(zip(geometry, codes)):
            inherited = code in {"_______", "KC_TRNS"}
            effective = layers[0][i] if inherited else code
            entries.append({"layout_index": i, **position, "keycode": code, "display_keycode": effective, "inherited": inherited})
        metadata["layers"].append({"layer": layer, "name": LAYER_INFO[layer][0], "keycode_counts": dict(Counter(codes)), "keys": entries})
    (args.output_dir / "ec-typek-satt-data.json").write_text(json.dumps(metadata, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    print(f"Verified {len(geometry)} keys × {len(layers)} layers; PNG: {'rendered' if converter else 'not rendered'}")


if __name__ == "__main__":
    main()
