from html.parser import HTMLParser
import tkinter as tk
from tkinter import ttk


TITLE_TO_TYPE = {
    "House": "commonHouse",
    "Infirmary": "hospital",
    "College": "college",
    "Column": "doricColumn",
    "Gazebo": "gazebo",
    "Fountain": "fountain",
    "Roadblock": "roadblock",
    "Food Vendor": "foodVendor",
    "Fleece Vendor": "fleeceVendor",
    "Oil Vendor": "oilVendor",
    "Tax Office": "taxOffice",
    "Avenue/Boulevard": "avenue",
    "Gymnasium": "gymnasium",
    "Theater": "theater",
    "Podium": "podium",
    "Monument": "commemorative",
    "Maintenance Office": "maintenanceOffice",
    "Park": "park",
    "Granary": "granary",
    "Storehouse": "warehouse",
    "Warehouse": "warehouse",
}

SRC_TO_TYPE = {
    "house2a.gif": "commonHouse",
    "infirmary.gif": "hospital",
    "college.gif": "college",
    "column.gif": "doricColumn",
    "gazebo.gif": "gazebo",
    "fountain.gif": "fountain",
    "road.gif": "road",
    "agora-road1.gif": "road",
    "agora-road2.gif": "road",
    "roadblock.gif": "roadblock",
    "agora-food.gif": "foodVendor",
    "agora-fleece.gif": "fleeceVendor",
    "agora-oil.gif": "oilVendor",
    "tax.gif": "taxOffice",
    "plaza.gif": "avenue",
    "gymnasium.gif": "gymnasium",
    "theater.gif": "theater",
    "podium.gif": "podium",
    "monument.gif": "commemorative",
    "maintenance.gif": "maintenanceOffice",
    "park2.gif": "park",
    "park3.gif": "park",
    "granary.gif": "granary",
    "storehouse.gif": "warehouse",
    "warehouse.gif": "warehouse",
}

TYPE_TO_GLYPH = {
    "commonHouse": "HH",
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
    "taxOffice": "TX",
    "avenue": "AV",
    "gymnasium": "GY",
    "theater": "TH",
    "podium": "PO",
    "commemorative": "CM",
    "maintenanceOffice": "MT",
    "park": "PK",
    "granary": "GR",
    "warehouse": "WH",
}

EXPAND_TO_TILES = {
    "park",
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
                 "maintenanceOffice", "park", "gazebo", "fountain",
                 "taxOffice", "podium"):
        return 2, 2
    if btype in ("warehouse", "college", "gymnasium", "commemorative"):
        return 3, 3
    if btype in ("granary", "hospital"):
        return 4, 4
    if btype == "theater":
        return 5, 5
    return 1, 1


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
    for element in elements:
        btype = element_type(element)
        x, y = element_xy(element)
        lines.append(f"        {{eBuildingType::{btype}, {x}, {y}}},")
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
