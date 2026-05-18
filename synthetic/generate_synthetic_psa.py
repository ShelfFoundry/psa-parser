#!/usr/bin/env python3

import json
import random
from pathlib import Path


OUT_DIR = Path(__file__).resolve().parent / "out"


def blank_row(length):
    return [""] * length


def setv(row, idx, value):
    if idx < len(row):
        row[idx] = str(value)


def row_to_line(row):
    return ",".join(row)


def gen_project(name, key_num, use_optional=True, escaped_comma=False):
    length = 216 if use_optional else 213
    r = blank_row(length)
    setv(r, 0, "Project")
    setv(r, 1, name)
    setv(r, 2, key_num)
    setv(r, 3, key_num)
    setv(r, 5, 7)
    setv(r, 6, 1.5)
    setv(r, 7, 1.5)
    setv(r, 8, 1)
    setv(r, 9, 1)
    setv(r, 12, 0)
    setv(r, 13, 1)
    setv(r, 185, "Synthetic project generated for parser validation")
    setv(r, 201, "Review")
    setv(r, 210, "synthetic-bot")
    setv(r, 211, "synthetic-bot")
    setv(r, 212, 0)
    if len(r) >= 214:
        setv(r, 213, "Weekly")
    if len(r) >= 215:
        payload = "alpha\\,beta\\,gamma" if escaped_comma else "synthetic_payload"
        setv(r, 214, payload)
    if len(r) >= 216:
        setv(r, 215, -1)
    return r


def gen_planogram(cfg, planogram_key):
    mode = cfg.get("planogram_tail", "base")
    if mode == "base":
        length = 229
    elif mode == "A":
        length = 233
    else:
        length = 255

    width = cfg.get("width", 96)
    height = cfg.get("height", 72)
    depth = cfg.get("depth", 24)

    r = blank_row(length)
    setv(r, 0, "Planogram")
    setv(r, 1, cfg["planogram_name"])
    setv(r, 2, planogram_key)
    setv(r, 3, width)
    setv(r, 4, height)
    setv(r, 5, depth)
    setv(r, 6, 16777088)
    setv(r, 7, 0.5)
    setv(r, 8, 1)
    setv(r, 9, width)
    setv(r, 10, 7)
    setv(r, 11, depth)
    setv(r, 12, 1)
    setv(r, 22, 0)
    setv(r, 23, 0)
    setv(r, 53, 0)
    setv(r, 54, 1)
    setv(r, 166, 0)
    setv(r, 169, 0)
    setv(r, 170, "synthetic_layout.psy")
    setv(r, 171, cfg.get("notes", "synthetic planogram"))
    setv(r, 182, 2)
    setv(r, 183, "Pending")
    setv(r, 194, "synthetic-bot")
    setv(r, 195, "synthetic-bot")
    setv(r, 197, 0.5)
    setv(r, 198, 12)
    setv(r, 199, 12)
    setv(r, 206, 1.5)
    setv(r, 207, 1.5)
    setv(r, 208, 1)
    setv(r, 209, 1)
    setv(r, 218, "WEEK 2026-20")
    setv(r, 219, f"BUS-{planogram_key}")
    setv(r, 220, cfg.get("department", "GROCERY"))
    setv(r, 223, cfg.get("custom_payload", ""))
    setv(r, 224, f"POG-{planogram_key:08d}-GUID")
    setv(r, 225, "DB-GUID-SYNTH")
    setv(r, 226, cfg.get("abbr", "SYN-POG"))
    setv(r, 227, cfg.get("category", "SYNTHETIC"))
    setv(r, 228, cfg.get("subcategory", "TEST"))

    if length > 229:
        setv(r, 229, 0)
        setv(r, 230, "ALLOC-GRP")
        setv(r, 231, 1)
        setv(r, 232, 0)
    if length > 233:
        setv(r, 233, -1)
        setv(r, 234, 0)
        setv(r, 235, 0)
        setv(r, 236, 0)
        setv(r, 237, "OK")
        setv(r, 238, 0)
        setv(r, 239, 0)
        setv(r, 240, "None")
        setv(r, 241, 0)
        setv(r, 242, 0)
        setv(r, 243, 0)
        setv(r, 244, 0)
        setv(r, 245, 0)
        setv(r, 246, 0)
        setv(r, 247, -1)
        setv(r, 248, -1)
        setv(r, 249, -1)
        setv(r, 250, -1)
        setv(r, 251, -1)
        setv(r, 252, "synthetic-server")
        setv(r, 253, 0)
        setv(r, 254, 0)

    return r


def gen_product(upc, biz_id, product_key, name, cfg, i):
    r = blank_row(274)
    setv(r, 0, "Product")
    setv(r, 1, upc)
    setv(r, 2, biz_id)
    setv(r, 3, name)
    setv(r, 4, product_key)
    setv(r, 5, round(2 + ((i * 3) % 6) * 0.3, 2))
    setv(r, 6, round(6 + ((i * 7) % 15) * 0.4, 2))
    setv(r, 7, round(1 + ((i * 11) % 8) * 0.35, 2))
    setv(r, 8, 11986095)
    setv(r, 9, f"ITEM{i:03d}")
    setv(r, 10, 1)
    setv(r, 11, "EA")
    setv(r, 12, "SYNMFG")
    setv(r, 13, cfg.get("category", "SYNTHETIC"))
    setv(r, 14, "SYNSUP")
    setv(r, 15, 1)
    setv(r, 19, 1)
    setv(r, 29, 0)
    setv(r, 33, round(1.0 + (i % 9) * 0.25, 2))
    setv(r, 35, 1)
    setv(r, 40, 0)
    setv(r, 113, 1)
    setv(r, 230, 0)
    setv(r, 232, "SYNTHETIC BRAND")
    setv(r, 233, cfg.get("subcategory", "TEST"))
    setv(r, 236, 0)
    setv(r, 249, "Live")
    setv(r, 258, "synthetic-bot")
    setv(r, 259, "synthetic-bot")
    setv(r, 261, -0.01)
    setv(r, 262, -0.01)
    setv(r, 264, f"PART-{i:04d}")
    setv(r, 270, cfg.get("product_custom_payload", ""))
    setv(r, 271, "DB-GUID-SYNTH")
    setv(r, 272, 0)
    setv(r, 273, 1000000000 + i)
    if cfg.get("add_product_extensions") and i % 4 == 0:
        setv(r, 114, "EXT-TEXT-A")
        setv(r, 164, 12.34)
        setv(r, 214, 1)
    return r


def gen_performance(upc, biz_id, perf_key, i, cfg):
    r = blank_row(150)
    setv(r, 0, "Performance")
    setv(r, 1, upc)
    setv(r, 2, biz_id)
    setv(r, 3, perf_key)
    setv(r, 4, round(0.99 + (i % 7) * 0.4, 2))
    setv(r, 5, 0)
    setv(r, 6, 1)
    setv(r, 7, round((i % 10) * 0.75, 2))
    setv(r, 8, 0)
    setv(r, 9, 0)
    setv(r, 40, 0)
    setv(r, 81, 1)
    setv(r, 82, 1)
    setv(r, 83, -0.01)
    setv(r, 84, -0.01)
    setv(r, 85, -1)
    setv(r, 86, -1)
    setv(r, 87, "WEEKLY")
    setv(r, 88, 1)
    setv(r, 89, 4)
    setv(r, 90, i % 100)
    setv(r, 96, f"PART-{i:04d}")
    setv(r, 141, cfg.get("performance_custom_payload", ""))
    setv(r, 142, -1)
    setv(r, 143, -1)
    setv(r, 144, 0)
    setv(r, 145, 0)
    setv(r, 149, 0)
    if cfg.get("add_performance_extensions") and i % 5 == 0:
        setv(r, 10, "P-EXT-TEXT")
        setv(r, 20, 4.56)
        setv(r, 30, 1)
    return r


def gen_segment(seg_key, width, x_offset, extended=False, payload=""):
    length = 52 if extended else 12
    r = blank_row(length)
    setv(r, 0, "Segment")
    setv(r, 1, "")
    setv(r, 2, seg_key)
    setv(r, 3, 0)
    setv(r, 4, width)
    setv(r, 5, 0)
    setv(r, 6, 0)
    setv(r, 7, 0)
    setv(r, 8, 0)
    setv(r, 9, 0)
    setv(r, 10, x_offset)
    setv(r, 11, 0)
    if extended:
        setv(r, 12, 0)
        setv(r, 13, 0)
        setv(r, 14, "SEG-EXT")
        setv(r, 24, 1)
        setv(r, 34, 0)
        setv(r, 44, 0)
        setv(r, 45, 0)
        setv(r, 46, 0)
        setv(r, 47, -1)
        setv(r, 48, 0)
        setv(r, 49, "PART-SEG")
        setv(r, 50, "")
        setv(r, 51, payload)
    return r


def gen_fixture(fx_key, name, x, width, y, z, depth, idx, include_optional=False):
    length = 160 if include_optional else 158
    r = blank_row(length)
    setv(r, 0, "Fixture")
    setv(r, 1, 0)
    setv(r, 2, name)
    setv(r, 3, fx_key)
    setv(r, 4, x)
    setv(r, 5, width)
    setv(r, 6, y)
    setv(r, 7, 1)
    setv(r, 8, z)
    setv(r, 9, depth)
    setv(r, 13, 0)
    setv(r, 24, 1)
    setv(r, 25, 1)
    setv(r, 33, -1)
    setv(r, 37, 1)
    setv(r, 45, "")
    setv(r, 46, "")
    setv(r, 49, 3)
    setv(r, 58, 3)
    setv(r, 67, 3)
    setv(r, 146, idx)
    setv(r, 147, 0)
    setv(r, 150, 0)
    setv(r, 154, 0)
    setv(r, 155, 0)
    setv(r, 156, "")
    setv(r, 157, f"PART-FX-{idx:03d}")
    if include_optional:
        setv(r, 158, 0)
        setv(r, 159, "")
    return r


def gen_position(pos_key, upc, biz_id, x, width, y, height, z, depth, facings, payload=""):
    r = blank_row(167)
    setv(r, 0, "Position")
    setv(r, 1, upc)
    setv(r, 2, biz_id)
    setv(r, 3, pos_key)
    setv(r, 4, x)
    setv(r, 5, width)
    setv(r, 6, y)
    setv(r, 7, height)
    setv(r, 8, z)
    setv(r, 9, depth)
    setv(r, 13, 0)
    setv(r, 14, facings)
    setv(r, 15, 1)
    setv(r, 16, 1)
    setv(r, 29, 4)
    setv(r, 42, "12T")
    setv(r, 43, 1)
    setv(r, 44, 1)
    setv(r, 45, 1)
    setv(r, 46, 1)
    setv(r, 47, 0)
    setv(r, 148, 0)
    setv(r, 149, 0)
    setv(r, 150, 0)
    setv(r, 154, -1)
    setv(r, 155, 0)
    setv(r, 156, 1)
    setv(r, 157, 4)
    setv(r, 160, 0)
    setv(r, 161, f"PART-POS-{pos_key}")
    setv(r, 162, 0)
    setv(r, 163, -1)
    setv(r, 164, payload)
    setv(r, 165, 0)
    setv(r, 166, 0)
    return r


def gen_drawing(draw_key, text, x=0, y=0, width=48, height=12, font="Arial", color=16777215):
    r = blank_row(48)
    setv(r, 0, "Drawing")
    setv(r, 1, 5)
    setv(r, 2, "")
    setv(r, 3, draw_key)
    setv(r, 4, x)
    setv(r, 5, width)
    setv(r, 6, y)
    setv(r, 7, height)
    setv(r, 8, 0)
    setv(r, 9, 0)
    setv(r, 10, 0)
    setv(r, 11, 1)
    setv(r, 12, color)
    setv(r, 13, 2)
    setv(r, 14, 0)
    setv(r, 15, 1)
    setv(r, 16, 0)
    setv(r, 17, 0)
    setv(r, 18, 0)
    setv(r, 19, 0)
    setv(r, 20, 0)
    setv(r, 21, 0)
    setv(r, 22, 0)
    setv(r, 23, text)
    setv(r, 24, 2)
    setv(r, 25, 1)
    setv(r, 26, 0)
    setv(r, 27, 10)
    setv(r, 28, 0)
    setv(r, 29, 0)
    setv(r, 30, 0)
    setv(r, 31, 400)
    setv(r, 32, 0)
    setv(r, 33, 0)
    setv(r, 34, 0)
    setv(r, 35, 0)
    setv(r, 36, 0)
    setv(r, 37, 0)
    setv(r, 38, 0)
    setv(r, 39, 0)
    setv(r, 40, font)
    setv(r, 41, x)
    setv(r, 42, y)
    setv(r, 43, 0)
    setv(r, 44, 1)
    setv(r, 45, 0)
    setv(r, 46, 0)
    setv(r, 47, 1)
    return r


def gen_divider(div_id, x, y, h, d, color=4210752):
    r = blank_row(18)
    setv(r, 0, "Divider")
    setv(r, 1, div_id)
    setv(r, 2, x)
    setv(r, 3, 0.06)
    setv(r, 4, y)
    setv(r, 5, h)
    setv(r, 6, 0)
    setv(r, 7, d)
    setv(r, 8, color)
    setv(r, 9, 0)
    setv(r, 13, 0)
    setv(r, 14, 0)
    setv(r, 15, 0)
    setv(r, 16, 0)
    setv(r, 17, 0)
    return r


FILES = [
    {"id": "0001", "name": "minimal_defaults", "seed": 1001, "products": 2, "segments": 1, "fixtures": 1, "positions": 2, "drawings": 0, "dividers": 0, "planogram_tail": "base", "segment_extended": False},
    {"id": "0002", "name": "small_realistic", "seed": 1002, "products": 8, "segments": 2, "fixtures": 3, "positions": 8, "drawings": 1, "dividers": 0, "planogram_tail": "base", "segment_extended": True},
    {"id": "0003", "name": "medium_realistic", "seed": 1003, "products": 24, "segments": 4, "fixtures": 8, "positions": 24, "drawings": 2, "dividers": 0, "planogram_tail": "base", "segment_extended": True, "add_product_extensions": True, "add_performance_extensions": True},
    {"id": "0004", "name": "large_realistic", "seed": 1004, "products": 60, "segments": 8, "fixtures": 16, "positions": 60, "drawings": 3, "dividers": 0, "planogram_tail": "base", "segment_extended": True, "add_product_extensions": True, "add_performance_extensions": True},
    {"id": "0005", "name": "planogram_tail_A", "seed": 1005, "products": 24, "segments": 4, "fixtures": 8, "positions": 24, "drawings": 1, "dividers": 0, "planogram_tail": "A", "segment_extended": True},
    {"id": "0006", "name": "planogram_tail_AB", "seed": 1006, "products": 24, "segments": 4, "fixtures": 8, "positions": 24, "drawings": 1, "dividers": 0, "planogram_tail": "AB", "segment_extended": True},
    {"id": "0007", "name": "escape_comma_cases", "seed": 1007, "products": 8, "segments": 2, "fixtures": 3, "positions": 8, "drawings": 1, "dividers": 0, "planogram_tail": "base", "segment_extended": True, "escaped_commas": True},
    {"id": "0008", "name": "multiline_drawing", "seed": 1008, "products": 8, "segments": 2, "fixtures": 3, "positions": 8, "drawings": 6, "dividers": 0, "planogram_tail": "base", "segment_extended": True, "multiline_drawings": True},
    {"id": "0009", "name": "divider_compat", "seed": 1009, "products": 8, "segments": 2, "fixtures": 3, "positions": 8, "drawings": 1, "dividers": 4, "planogram_tail": "base", "segment_extended": True},
    {"id": "0010", "name": "sparse_and_trailing", "seed": 1010, "products": 12, "segments": 2, "fixtures": 4, "positions": 12, "drawings": 0, "dividers": 0, "planogram_tail": "base", "segment_extended": False, "sparse": True},
]


def make_filename(cfg):
    ts = f"2026_05_18_10_00_{cfg['id'][-2:]}"
    return f"SYN_{cfg['id']}_{cfg['name']}_{ts}.psa"


def build_file(cfg):
    random.seed(cfg["seed"])
    lines = [
        "PROSPACE SCHEMATIC FILE",
        "; Version 2018.1.0",
        "; Codepage=1252",
    ]

    planogram_key = 9000000 + int(cfg["id"])
    project_key = 8000000 + int(cfg["id"])

    cfg = {**cfg}
    cfg.setdefault("planogram_name", f"SYN PLANOGRAM {cfg['id']}")
    cfg.setdefault("department", random.choice(["GROCERY", "HEALTH", "PETS", "PRODUCE"]))
    cfg.setdefault("category", random.choice(["SYNTHETIC", "BEVERAGES", "PERSONAL CARE", "PET FOOD"]))
    cfg.setdefault("subcategory", random.choice(["TEST", "ENDCAP", "AISLE", "SEASONAL"]))
    if cfg.get("escaped_commas"):
        cfg["custom_payload"] = "phase\\,alpha\\,test"
        cfg["product_custom_payload"] = "prod\\,payload"
        cfg["performance_custom_payload"] = "perf\\,payload"

    lines.append(row_to_line(gen_project(f"Synthetic Project {cfg['id']}", project_key, use_optional=True, escaped_comma=cfg.get("escaped_commas", False))))
    lines.append(row_to_line(gen_planogram(cfg, planogram_key)))

    products = []
    for i in range(cfg["products"]):
        upc = f"9{cfg['id']}{i:07d}"[:12]
        biz = 7000000 + int(cfg["id"]) * 1000 + i
        pkey = 6000000 + int(cfg["id"]) * 1000 + i
        name = f"SYN PRODUCT {cfg['id']}-{i:03d}"
        products.append((upc, biz, pkey, name))
        line = row_to_line(gen_product(upc, biz, pkey, name, cfg, i))
        if cfg.get("sparse"):
            line += ",,,"
        lines.append(line)

    for i, (upc, biz, _, _) in enumerate(products):
        perf_key = 800000000 + int(cfg["id"]) * 1000 + i
        lines.append(row_to_line(gen_performance(upc, biz, perf_key, i, cfg)))

    seg_width = max(12, int(cfg.get("width", 96) / max(1, cfg["segments"])))
    for i in range(cfg["segments"]):
        seg_key = 5000000 + int(cfg["id"]) * 100 + i
        payload = "seg\\,payload" if cfg.get("escaped_commas") and i == 0 else ""
        lines.append(row_to_line(gen_segment(seg_key, seg_width, i * seg_width, cfg.get("segment_extended", False), payload=payload)))

    for i in range(cfg["fixtures"]):
        fx_key = 4000000 + int(cfg["id"]) * 100 + i
        include_optional = i == 0
        x = (i % max(1, cfg["segments"])) * seg_width
        y = 7 + (i // max(1, cfg["segments"])) * 15
        lines.append(row_to_line(gen_fixture(fx_key, f"SYN FIXTURE {i+1}", x, seg_width, y, 0, 24, i + 1, include_optional=include_optional)))

    for i in range(cfg["positions"]):
        upc, biz, _, _ = products[i % len(products)]
        pos_key = 3000000 + int(cfg["id"]) * 1000 + i
        x = round((i % max(1, cfg["segments"])) * seg_width + (i % 4) * 2.5, 2)
        y = round(8 + (i % max(1, cfg["fixtures"])) * 1.5, 2)
        width = round(2 + (i % 5) * 0.4, 2)
        height = round(6 + (i % 4) * 1.2, 2)
        depth = round(1 + (i % 3) * 0.5, 2)
        payload = "pos\\,payload" if cfg.get("escaped_commas") and i == 1 else ""
        line = row_to_line(gen_position(pos_key, upc, biz, x, width, y, height, 0, depth, 1 + (i % 3), payload=payload))
        if cfg.get("sparse") and i % 3 == 0:
            line += ",,"
        lines.append(line)

    for i in range(cfg["drawings"]):
        draw_key = 2000000 + int(cfg["id"]) * 100 + i
        if cfg.get("multiline_drawings"):
            text = f"SYN NOTE {i+1}\\r\\nLINE TWO\\r\\nLINE THREE"
        elif cfg.get("escaped_commas"):
            text = "CHECK\\,VERIFY\\,SIGN"
        else:
            text = f"SYN DRAWING NOTE {i+1}"
        font = "Tahoma" if i % 2 else "Arial"
        lines.append(row_to_line(gen_drawing(draw_key, text, x=i * 12, y=-5 + i, width=48, height=12, font=font)))

    for i in range(cfg["dividers"]):
        div_id = 69000 + int(cfg["id"]) * 10 + i
        lines.append(row_to_line(gen_divider(div_id, 0, 8 + i * 12, 3, 15)))

    filename = make_filename(cfg)
    file_path = OUT_DIR / filename
    with file_path.open("w", encoding="cp1252", newline="") as f:
        f.write("\r\n".join(lines) + "\r\n")

    counts = {
        "Project": 1,
        "Planogram": 1,
        "Product": cfg["products"],
        "Performance": cfg["products"],
        "Segment": cfg["segments"],
        "Fixture": cfg["fixtures"],
        "Position": cfg["positions"],
        "Drawing": cfg["drawings"],
        "Divider": cfg["dividers"],
    }
    return filename, counts


def main():
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    manifest = []

    for cfg in FILES:
        filename, counts = build_file(cfg)
        manifest.append(
            {
                "file": filename,
                "seed": cfg["seed"],
                "archetype": cfg["name"],
                "planogram_tail": cfg.get("planogram_tail", "base"),
                "segment_extended": cfg.get("segment_extended", False),
                "escaped_commas": cfg.get("escaped_commas", False),
                "multiline_drawings": cfg.get("multiline_drawings", False),
                "counts": counts,
            }
        )

    manifest_path = OUT_DIR / "manifest.json"
    manifest_path.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")

    lines = [
        "# Synthetic PSA Set",
        "",
        "This folder contains 10 synthetic PSA files generated from `psa-spec` requirements.",
        "",
        "## Notes",
        "",
        "- All files are synthetic and generated with deterministic seeds.",
        "- Line endings are CRLF and encoding is Windows-1252-compatible.",
        "- Optional parser paths are covered across the set (planogram tails, segment extension, drawings, divider, escaped comma cases).",
        "- See `manifest.json` for per-file counts and options.",
        "",
        "## Files",
        "",
    ]
    for item in manifest:
        c = item["counts"]
        lines.append(
            f"- `{item['file']}` (seed={item['seed']}): "
            f"Pj={c['Project']}, Pg={c['Planogram']}, Pr={c['Product']}, Pf={c['Performance']}, "
            f"Sg={c['Segment']}, Fx={c['Fixture']}, Ps={c['Position']}, Dr={c['Drawing']}, Dv={c['Divider']}"
        )

if __name__ == "__main__":
    main()
