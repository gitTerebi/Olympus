from html.parser import HTMLParser
import tkinter as tk
from tkinter import ttk


TITLE_TO_TYPE = {
    "House": "commonHouse",
    "Townhouse": "commonHouse",
    "Estate": "eliteHousing",
    "Infirmary": "hospital",
    "College": "college",
    "Column": "doricColumn",
    "Gazebo": "gazebo",
    "Fountain": "fountain",
    "Roadblock": "roadblock",
    "Food Vendor": "foodVendor",
    "Fleece Vendor": "fleeceVendor",
    "Oil Vendor": "oilVendor",
    "Arms Vendor": "armsVendor",
    "Wine Vendor": "wineVendor",
    "Horse Trainer": "horseTrainer",
    "Tax Office": "taxOffice",
    "Avenue/Boulevard": "avenue",
    "Gymnasium": "gymnasium",
    "Theater": "theater",
    "Drama School": "dramaSchool",
    "Podium": "podium",
    "Stadium": "stadium",
    "Monument": "commemorative",
    "Maintenance Office": "maintenanceOffice",
    "Park": "park",
    "Bench": "bench",
    "Hedge Maze": "hedgeMaze",
    "Fish Pond": "fishPond",
    "Granary": "granary",
    "Storehouse": "warehouse",
    "Warehouse": "warehouse",
    "Trading Post": "tradePost",
    "Pier": "pier",
    "Farm": "wheatFarm",
    "Carding Shed": "cardingShed",
    "Dairy": "dairy",
    "Growers' Lodge": "growersLodge",
    "Olive tree": "oliveTree",
    "Grapevine": "vine",
    "Hunting Lodge": "huntingLodge",
    "Fishing Wharf": "fishery",
    "Urchin Quay": "urchinQuay",
    "Timber Mill": "timberMill",
    "Masonry Shop": "masonryShop",
    "Foundry": "foundry",
    "Mint": "mint",
    "Olive Press": "olivePress",
    "Winery": "winery",
    "Artisans' Guild": "artisansGuild",
    "Wall": "wall",
    "Tower": "tower",
    "Armory": "armory",
    "Horse Ranch": "horseRanch",
    "Palace": "palace",
    "Bibliotheke": "bibliotheke",
    "Inventors' Workshop": "inventorsWorkshop",
    "Laboratory": "laboratory",
    "University": "university",
    "Observatory": "observatory",
    "Museum": "museum",
}

SRC_TO_TYPE = {
    "house2a.gif": "commonHouse",
    "house2a_p.gif": "commonHouse",
    "house4a.gif": "eliteHousing",
    "house4a_p.gif": "eliteHousing",
    "infirmary.gif": "hospital",
    "college.gif": "college",
    "column.gif": "doricColumn",
    "gazebo.gif": "gazebo",
    "fountain.gif": "fountain",
    "road.gif": "road",
    "agora-road1.gif": "road",
    "agora-road2.gif": "road",
    "agora-empty.gif": None,
    "roadblock.gif": "roadblock",
    "agora-food.gif": "foodVendor",
    "agora-fleece.gif": "fleeceVendor",
    "agora-oil.gif": "oilVendor",
    "agora-arms.gif": "armsVendor",
    "agora-wine.gif": "wineVendor",
    "agora-horses.gif": "horseTrainer",
    "tax.gif": "taxOffice",
    "plaza.gif": "avenue",
    "gymnasium.gif": "gymnasium",
    "theater.gif": "theater",
    "theaterschool.gif": "dramaSchool",
    "podium.gif": "podium",
    "stadium-v.gif": "stadium",
    "monument.gif": "commemorative",
    "maintenance.gif": "maintenanceOffice",
    "park1.gif": "park",
    "park2.gif": "park",
    "park3.gif": "park",
    "bench.gif": "bench",
    "hedgemaze.gif": "hedgeMaze",
    "fishpond.gif": "fishPond",
    "granary.gif": "granary",
    "storehouse.gif": "warehouse",
    "warehouse.gif": "warehouse",
    "tradepost.gif": "tradePost",
    "pier2.gif": "pier",
    "wheatfarm.gif": "wheatFarm",
    "cardingshed.gif": "cardingShed",
    "cheesemaker.gif": "dairy",
    "lodge.gif": "growersLodge",
    "olivetree.gif": "oliveTree",
    "vines.gif": "vine",
    "boarhunter.gif": "huntingLodge",
    "fishery.gif": "fishery",
    "urchins.gif": "urchinQuay",
    "timber.gif": "timberMill",
    "masonry.gif": "masonryShop",
    "foundry.gif": "foundry",
    "mint.gif": "mint",
    "olivepress.gif": "olivePress",
    "winepress.gif": "winery",
    "artisan.gif": "artisansGuild",
    "wall-h.gif": "wall",
    "tower.gif": "tower",
    "armourer.gif": "armory",
    "horseranch2.gif": "horseRanch",
    "horsecorral2.gif": "horseRanchEnclosure",
    "palace-v.gif": "palace",
    "bibliotheke.gif": "bibliotheke",
    "inventor.gif": "inventorsWorkshop",
    "laboratory.gif": "laboratory",
    "university.gif": "university",
    "observatory.gif": "observatory",
    "museum.gif": "museum",
}

TYPE_TO_GLYPH = {
    "commonHouse": "HH",
    "eliteHousing": "EH",
    "hospital": "HP",
    "college": "CO",
    "doricColumn": "CL",
    "gazebo": "GZ",
    "fountain": "FO",
    "road": "##",
    "roadblock": "RB",
    "foodVendor": "FD",
    "fleeceVendor": "FL",
    "oilVendor": "OL",
    "armsVendor": "AM",
    "wineVendor": "WV",
    "horseTrainer": "HT",
    "taxOffice": "TX",
    "avenue": "AV",
    "gymnasium": "GY",
    "theater": "TH",
    "dramaSchool": "DS",
    "podium": "PO",
    "stadium": "ST",
    "commemorative": "CM",
    "maintenanceOffice": "MT",
    "park": "PK",
    "bench": "BN",
    "hedgeMaze": "HM",
    "fishPond": "FP",
    "granary": "GR",
    "warehouse": "WH",
    "tradePost": "TP",
    "pier": "PI",
    "wheatFarm": "WF",
    "cardingShed": "CS",
    "dairy": "DY",
    "growersLodge": "GL",
    "oliveTree": "OT",
    "vine": "VI",
    "huntingLodge": "HL",
    "fishery": "FI",
    "urchinQuay": "UQ",
    "timberMill": "TM",
    "masonryShop": "MS",
    "foundry": "FN",
    "mint": "MN",
    "olivePress": "OP",
    "winery": "WN",
    "artisansGuild": "AG",
    "wall": "WL",
    "tower": "TW",
    "armory": "AR",
    "horseRanch": "HR",
    "horseRanchEnclosure": "HE",
    "palace": "PC",
    "bibliotheke": "BL",
    "inventorsWorkshop": "IW",
    "laboratory": "LB",
    "university": "UN",
    "observatory": "OB",
    "museum": "MU",
}

EXPAND_TO_TILES = {
    "park",
    "bench",
}

AGORA_ORIENTATION_ID = {
    "bottomRight": 0,
    "topLeft": 1,
    "bottomLeft": 2,
    "topRight": 3,
}


class StampHtmlParser(HTMLParser):
    def __init__(self):
        super().__init__()
        self.rows = []
        self.current_row = None
        self.current_cell = None
        self.in_td = False

    def handle_starttag(self, tag, attrs):
        attrs = dict(attrs)
        if tag == "tr":
            self.current_row = []
        elif tag == "td" and self.current_row is not None:
            self.in_td = True
            self.current_cell = {
                "colspan": int(attrs.get("colspan", "1")),
                "rowspan": int(attrs.get("rowspan", "1")),
                "type": None,
            }
        elif tag == "img" and self.in_td and self.current_cell is not None:
            title = attrs.get("title", "")
            src = attrs.get("src", "").replace("\\", "/").split("/")[-1]
            self.current_cell["type"] = TITLE_TO_TYPE.get(title) or SRC_TO_TYPE.get(src)

    def handle_endtag(self, tag):
        if tag == "td" and self.current_row is not None and self.current_cell is not None:
            self.current_row.append(self.current_cell)
            self.current_cell = None
            self.in_td = False
        elif tag == "tr" and self.current_row is not None:
            self.rows.append(self.current_row)
            self.current_row = None


def parse_blueprint(html):
    parser = StampHtmlParser()
    parser.feed(html)

    occupied = {}
    elements = []

    for y, row in enumerate(parser.rows):
        x = 0
        while (x, y) in occupied:
            x += 1

        for cell in row:
            while (x, y) in occupied:
                x += 1

            btype = cell["type"]
            colspan = cell["colspan"]
            rowspan = cell["rowspan"]
            if btype:
                if btype in EXPAND_TO_TILES:
                    for dy in range(rowspan):
                        for dx in range(colspan):
                            elements.append((btype, x + dx, y + dy, 1, 1))
                else:
                    elements.append((btype, x, y, colspan, rowspan))

            for dy in range(rowspan):
                for dx in range(colspan):
                    occupied[(x + dx, y + dy)] = btype or ""

            x += colspan

    return elements


def element_type(element):
    return element[0]


def element_xy(element):
    return element[1], element[2]


def element_size(element):
    if len(element) >= 5:
        return element[3], element[4]

    btype = element_type(element)
    if btype in ("commonHouse", "foodVendor", "fleeceVendor", "oilVendor",
                 "armsVendor", "wineVendor", "horseTrainer",
                 "maintenanceOffice", "gazebo", "fountain",
                 "taxOffice", "podium", "bibliotheke",
                 "cardingShed", "dairy", "growersLodge", "huntingLodge",
                 "fishery", "urchinQuay", "timberMill", "masonryShop",
                 "foundry", "mint", "olivePress", "winery",
                 "artisansGuild", "tower", "armory", "pier"):
        return 2, 2
    if btype in ("warehouse", "college", "gymnasium", "commemorative",
                 "hedgeMaze", "dramaSchool", "horseRanch",
                 "inventorsWorkshop", "university", "wheatFarm"):
        return 3, 3
    if btype in ("granary", "hospital", "eliteHousing", "fishPond",
                 "tradePost", "horseRanchEnclosure", "laboratory"):
        return 4, 4
    if btype in ("theater", "observatory"):
        return 5, 5
    return 1, 1


def anchor_offset_for_size(sw, sh):
    if sw == 2 and sh == 2:
        return 0, 1
    if sw == 3 and sh == 3:
        return 1, 1
    if sw == 4 or sh == 4:
        return 1, 2
    if sw == 5 or sh == 5:
        return 2, 2
    if sw == 6 or sh == 6:
        return 2, 2
    return 0, 0


def element_anchor_xy(element):
    x, y = element_xy(element)
    sw, sh = element_size(element)
    dx, dy = anchor_offset_for_size(sw, sh)
    return x + dx, y + dy


def explicit_agoras(elements):
    roads = {element_xy(element) for element in elements
             if element_type(element) == "road"}
    vendor_types = {"foodVendor", "fleeceVendor", "oilVendor",
                    "armsVendor", "wineVendor", "horseTrainer"}
    vendors = [element for element in elements
               if element_type(element) in vendor_types]

    # all tiles occupied by non-road, non-vendor buildings
    blocked = set()
    for element in elements:
        btype = element_type(element)
        if btype in vendor_types or btype == "road":
            continue
        x, y = element_xy(element)
        sw, sh = element_size(element)
        for dy in range(sh):
            for dx in range(sw):
                blocked.add((x + dx, y + dy))

    candidates = []

    def vendor_cells(vendor):
        x, y = element_xy(vendor)
        sw, sh = element_size(vendor)
        return {(x + dx, y + dy) for dx in range(sw) for dy in range(sh)}

    vendor_footprints = [vendor_cells(vendor) for vendor in vendors]

    def add(x, y, orientation, spaces):
        space_set = set(spaces)
        if space_set & blocked:
            return
        covered = {i for i, cells in enumerate(vendor_footprints)
                   if cells & space_set}
        if covered:
            candidates.append((len(covered), covered,
                               ("commonAgora", x, y, 1, 1,
                                AGORA_ORIENTATION_ID[orientation])))

    for element in vendors:
        x0, y0 = element_xy(element)
        sw, sh = element_size(element)

        for start in range(x0 - 4, x0 + sw):
            run = [(start + i, y0 - 1) for i in range(6)]
            if all(tile in roads for tile in run):
                spaces = [(start + i, y0 + dy)
                          for i in range(6) for dy in range(2)]
                add(start, y0 - 1, "bottomLeft", spaces)
                break

        for start in range(x0 - 4, x0 + sw):
            run = [(start + i, y0 + sh) for i in range(6)]
            if all(tile in roads for tile in run):
                spaces = [(start + i, y0 + sh - 2 + dy)
                          for i in range(6) for dy in range(2)]
                add(start, y0 + sh - 2, "topRight", spaces)
                break

        for start in range(y0 - 4, y0 + sh):
            run = [(x0 - 1, start + i) for i in range(6)]
            if all(tile in roads for tile in run):
                spaces = [(x0 + dx, start + i)
                          for i in range(6) for dx in range(2)]
                add(x0 - 1, start, "bottomRight", spaces)
                break

        for start in range(y0 - 4, y0 + sh):
            run = [(x0 + sw, start + i) for i in range(6)]
            if all(tile in roads for tile in run):
                spaces = [(x0 + sw - 2 + dx, start + i)
                          for i in range(6) for dx in range(2)]
                add(x0 + sw - 2, start, "topLeft", spaces)
                break

    result = []
    covered_vendors = set()
    seen = set()
    for _, covered, agora in sorted(candidates, key=lambda c: c[0], reverse=True):
        if covered <= covered_vendors:
            continue
        key = agora[1], agora[2], agora[5]
        if key in seen:
            continue
        seen.add(key)
        covered_vendors |= covered
        result.append(agora)
    return result


def element_sizes(elements):
    result = {}
    for element in elements:
        result[element] = element_size(element)
    return result


def to_ascii_preview(elements):
    if not elements:
        return ""

    sizes = element_sizes(elements)
    width = 0
    height = 0
    cells = {}

    for element in elements:
        btype = element_type(element)
        x, y = element_xy(element)
        sw, sh = sizes[element]
        glyph = TYPE_TO_GLYPH.get(btype, "??")
        width = max(width, x + sw)
        height = max(height, y + sh)
        for yy in range(y, y + sh):
            for xx in range(x, x + sw):
                cells[(xx, yy)] = glyph

    lines = ["    " + " ".join(f"{x:02}" for x in range(width))]
    for y in range(height):
        row = [cells.get((x, y), "..") for x in range(width)]
        lines.append(f"{y:02}  " + " ".join(row))

    legend_items = []
    for btype, glyph in TYPE_TO_GLYPH.items():
        if any(element_type(element) == btype for element in elements):
            legend_items.append(f"{glyph}={btype}")
    lines.append("")
    lines.append(" ".join(legend_items))
    return "\n".join(lines)


def to_cpp(elements):
    lines = []
    for element in explicit_agoras(elements) + elements:
        btype = element_type(element)
        if btype == "commonAgora" and len(element) >= 6:
            x, y = element_xy(element)
            lines.append(f"{{eBuildingType::{btype}, {x}, {y}, {element[5]}}},")
        else:
            x, y = element_anchor_xy(element)
            lines.append(f"{{eBuildingType::{btype}, {x}, {y}}},")
    if lines:
        lines[-1] = lines[-1].rstrip(",")
    return "\n".join(lines)


def convert():
    html = input_text.get("1.0", "end")
    try:
        elements = parse_blueprint(html)
        output_text.delete("1.0", "end")
        output_text.insert("1.0", to_cpp(elements))
        preview_text.delete("1.0", "end")
        preview_text.insert("1.0", to_ascii_preview(elements))
        status_var.set(f"{len(elements)} blueprint entries")
    except Exception as err:
        status_var.set(str(err))


def main():
    global input_text
    global output_text
    global preview_text
    global status_var

    root = tk.Tk()
    root.title("Stamp HTML Blueprint Converter")
    root.geometry("980x720")

    root.columnconfigure(0, weight=1)
    root.columnconfigure(1, weight=1)
    root.rowconfigure(1, weight=3)
    root.rowconfigure(3, weight=2)

    ttk.Label(root, text="HTML").grid(row=0, column=0, sticky="w", padx=8, pady=(8, 2))
    ttk.Label(root, text="C++ Blueprint").grid(row=0, column=1, sticky="w", padx=8, pady=(8, 2))

    input_text = tk.Text(root, wrap="none", undo=True)
    output_text = tk.Text(root, wrap="none", undo=True)
    input_text.grid(row=1, column=0, sticky="nsew", padx=(8, 4), pady=4)
    output_text.grid(row=1, column=1, sticky="nsew", padx=(4, 8), pady=4)

    ttk.Label(root, text="MONO Preview").grid(row=2, column=0, columnspan=2, sticky="w", padx=8, pady=(8, 2))
    preview_text = tk.Text(root, wrap="none", undo=True, font=("Courier New", 10))
    preview_text.grid(row=3, column=0, columnspan=2, sticky="nsew", padx=8, pady=4)

    button_frame = ttk.Frame(root)
    button_frame.grid(row=4, column=0, columnspan=2, sticky="ew", padx=8, pady=8)
    button_frame.columnconfigure(1, weight=1)

    ttk.Button(button_frame, text="Convert", command=convert).grid(row=0, column=0, sticky="w")
    status_var = tk.StringVar(value="Paste HTML, then Convert")
    ttk.Label(button_frame, textvariable=status_var).grid(row=0, column=1, sticky="w", padx=12)

    root.mainloop()


if __name__ == "__main__":
    main()
