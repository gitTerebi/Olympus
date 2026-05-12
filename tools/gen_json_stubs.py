"""
Scan all serialize(eSaveArchive&) methods, extract field names,
generate serializeJson() stub skeletons showing what needs porting.
Output: tools/json_stubs_report.txt
"""
import re, glob, os, collections

ROOT = "c:/Users/somtam/Downloads/_repo/eZeus"

def read_file(p):
    return open(p, encoding='utf-8', errors='ignore').read()

def get_body(txt, m):
    i = m.end()
    while i < len(txt) and txt[i] != '{': i += 1
    if i >= len(txt): return ""
    depth = 1; i += 1; start = i
    while i < len(txt) and depth:
        if txt[i]=='{': depth+=1
        elif txt[i]=='}': depth-=1
        i+=1
    return txt[start:i-1]

def classify_line(line):
    s = line.strip()
    if re.match(r'ar\.field\(', s):
        m = re.match(r'ar\.field\("([^"]+)"', s)
        name = m.group(1) if m else '?'
        return ('field', name, s)
    if re.search(r'\.read\(|\.write\(', s):
        return ('raw', None, s)
    if re.match(r'for\s*\(', s):
        return ('loop', None, s)
    if re.match(r'if\s*\(ar\.reading', s) or re.match(r'if\s*\(ar\.writing', s):
        return ('split', None, s)
    return ('other', None, s)

cpp = [f for f in glob.glob(os.path.join(ROOT,'**/*.cpp'),recursive=True) if '.kilo' not in f]
h   = [f for f in glob.glob(os.path.join(ROOT,'**/*.h'),  recursive=True) if '.kilo' not in f]

results = []  # (file, class, fields, raw_ops, has_split, has_loop)

for f in cpp + h:
    txt = read_file(f)
    short = f.replace(ROOT,'').replace('\\','/').lstrip('/')
    for m in re.finditer(r'void\s+(\w+)::serialize\s*\(eSaveArchive[^)]*\)', txt):
        cls = m.group(1)
        body = get_body(txt, m)
        fields = []
        raw_ops = []
        has_split = False
        has_loop  = False
        for line in body.splitlines():
            kind, name, text = classify_line(line)
            if kind == 'field':   fields.append(name)
            elif kind == 'raw':   raw_ops.append(text.strip()[:80])
            elif kind == 'split': has_split = True
            elif kind == 'loop':  has_loop  = True
        results.append((short, cls, fields, raw_ops, has_split, has_loop))

# check if serializeJson already exists
json_classes = set()
for f in cpp + h:
    txt = read_file(f)
    for m in re.finditer(r'void\s+(\w+)::serializeJson\s*\(', txt):
        json_classes.add(m.group(1))

lines = []
lines.append("SERIALIZE -> serializeJson PORTING REPORT")
lines.append("=" * 70)
lines.append(f"Total serialize() methods found: {len(results)}")
lines.append(f"Already have serializeJson():    {len(json_classes)}")
lines.append(f"Remaining to port:               {len(results) - len([r for r in results if r[1] in json_classes])}")
lines.append("")

# group: has raw ops (needs manual), clean (auto-portble), has split
clean   = [r for r in results if not r[3] and not r[4] and r[1] not in json_classes]
raw     = [r for r in results if r[3]      and r[1] not in json_classes]
split   = [r for r in results if r[4]      and r[1] not in json_classes]
done    = [r for r in results if r[1] in json_classes]

lines.append(f"CLEAN (auto-portable, {len(clean)} classes):")
for short, cls, fields, _, _, has_loop in clean:
    loop_tag = " [has loop]" if has_loop else ""
    lines.append(f"  {cls:40s} {len(fields):3d} fields{loop_tag}  {short}")

lines.append("")
lines.append(f"HAS RAW OPS (manual, {len(raw)} classes):")
for short, cls, fields, raw_ops, _, _ in raw:
    lines.append(f"  {cls:40s} {len(fields):3d} fields  {short}")
    for op in raw_ops[:3]:
        lines.append(f"    raw: {op}")
    if len(raw_ops) > 3:
        lines.append(f"    ... +{len(raw_ops)-3} more")

lines.append("")
lines.append(f"HAS SPLIT read/write (needs merge, {len(split)} classes):")
for short, cls, fields, _, _, _ in split:
    lines.append(f"  {cls:40s} {len(fields):3d} fields  {short}")

lines.append("")
lines.append(f"ALREADY DONE ({len(done)} classes):")
for short, cls, *_ in done:
    lines.append(f"  {cls:40s}  {short}")

report = '\n'.join(lines)
print(report)
out = os.path.join(ROOT, 'tools', 'json_stubs_report.txt')
with open(out, 'w', encoding='utf-8') as f:
    f.write(report)
print(f"\n-> {out}")
