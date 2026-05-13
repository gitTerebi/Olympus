#!/usr/bin/env python3
"""
Post-pass: resolve '// TODO: serialize(ar)' in serializeJson bodies
by finding ClassName::serialize(eSaveArchive&) in .cpp and inlining fields.
Also handles a few other patterns.
"""

import re
from pathlib import Path

ROOT = Path(r"C:\Users\somtam\Downloads\_repo\eZeus")

def strip_const_cast(s):
    return re.sub(r'const_cast<[^>]+>\(([^)]+)\)', r'\1', s)

def find_brace_end(text, start):
    depth = 1
    p = start + 1
    while p < len(text) and depth > 0:
        if text[p] == '{': depth += 1
        elif text[p] == '}': depth -= 1
        p += 1
    return p

def extract_top_stmts(body):
    stmts, cur, depth = [], [], 0
    for c in body.strip():
        if c == '{': depth += 1; cur.append(c)
        elif c == '}': depth -= 1; cur.append(c)
        elif c == ';' and depth == 0:
            s = ''.join(cur).strip()
            if s: stmts.append(s)
            cur = []
        else: cur.append(c)
    s = ''.join(cur).strip()
    if s: stmts.append(s)
    return stmts

def translate_serialize_body(body, indent='        '):
    """Translate serialize(eSaveArchive&) body to serializeJson lines."""
    flat = re.sub(r'\s+', ' ', body).strip()
    stmts = extract_top_stmts(flat)
    out = []
    seen_bases = set()
    for s in stmts:
        s = s.strip()
        if not s: continue

        # base::serialize(ar) or base::serializeJson(ar)
        m = re.match(r'([\w:]+)::(serialize|serializeJson)\s*\(', s)
        if m:
            base = m.group(1)
            if base not in seen_bases:
                out.append(f'{indent}{base}::serializeJson(ar);')
                seen_bases.add(base)
            continue

        # eSaveArchive constructor
        if re.match(r'eSaveArchive\s+\w+\s*\(', s): continue

        # ar.field / archive.field
        m = re.match(r'\w+\.field\s*\(\s*"([^"]+)"\s*,\s*(.+)\)', s)
        if m:
            key = m.group(1)
            val = strip_const_cast(m.group(2)).strip()
            out.append(f'{indent}ar.field("{key}", {val});')
            continue

        # for loop with ar.field inside — keep as-is but replace ar with ar
        if s.startswith('for(') or s.startswith('for ('):
            # just emit a comment — too complex for auto
            out.append(f'{indent}// TODO(loop): {s[:120]}')
            continue

        # if(ar.reading()) / if(ar.writing()) blocks — translate recursively
        m = re.match(r'if\s*\(\s*ar\.reading\s*\(\s*\)\s*\)\s*(\{.+\})\s*else\s*(\{.+\})', s)
        if m:
            # already split — reconstruct
            out.append(f'{indent}// TODO(conditional): {s[:120]}')
            continue

        # readCity — city cross-reference
        m = re.match(r'ar\.readStream\(\)\.readCity\s*\(\w+,\s*\[this\]\s*\(.*?\)\s*\{([^}]+)\}\)', s)
        if m:
            assign = m.group(1).strip().rstrip(';')
            mem = re.match(r'(m\w+)\s*=', assign)
            if mem:
                member = mem.group(1)
                out.append(f'{indent}ar.cityRef("{member}", {member}, *gameBoard());')
                continue

        # mForces.read / mForces.write — complex, leave TODO
        if 'mForces' in s or '.read(' in s or '.write(' in s:
            out.append(f'{indent}// TODO(complex): {s[:120]}')
            continue

        out.append(f'{indent}// TODO: {s[:120]}')

    return out

def get_serialize_body(class_name, cpp_text):
    """Find ClassName::serialize(eSaveArchive body in cpp_text."""
    pattern = re.escape(class_name) + r'\s*::\s*serialize\s*\(\s*eSaveArchive'
    m = re.search(pattern, cpp_text)
    if not m:
        return None
    ob = cpp_text.find('{', m.end())
    if ob < 0: return None
    end = find_brace_end(cpp_text, ob)
    return cpp_text[ob+1:end-1]

def process_file(path):
    text = path.read_text(encoding='utf-8', errors='replace')
    if '// TODO: serialize(ar)' not in text:
        return False

    original = text
    cpp_text = ''
    cpp = path.with_suffix('.cpp')
    if cpp.exists():
        cpp_text = cpp.read_text(encoding='utf-8', errors='replace')

    # find class names that have TODO: serialize(ar)
    # scan serializeJson bodies for the TODO
    sj_re = re.compile(
        r'void\s+serializeJson\s*\(\s*eJsonArchive\s*&\s*\w+\s*\)'
        r'(?:\s*override)?\s*\{([^}]*// TODO: serialize\(ar\)[^}]*)\}',
        re.DOTALL
    )

    replacements = []
    for sm in sj_re.finditer(text):
        body = sm.group(1)
        stub_start = sm.start(1)
        stub_end = sm.end(1)

        # find class name (last 'class X' before this point)
        preceding = text[:sm.start()]
        class_m = None
        for cm in re.finditer(r'\bclass\s+(\w+)', preceding):
            class_m = cm
        if not class_m:
            continue
        class_name = class_m.group(1)

        serialize_body = get_serialize_body(class_name, cpp_text)
        if not serialize_body:
            continue

        # get indent from existing body
        indent_m = re.search(r'\n(\s+)', body)
        indent = indent_m.group(1) if indent_m else '        '

        # extract non-TODO lines to keep
        existing_lines = []
        for line in body.splitlines():
            stripped = line.strip()
            if stripped and '// TODO: serialize(ar)' not in stripped:
                existing_lines.append(line)

        new_lines = translate_serialize_body(serialize_body, indent)
        all_lines = existing_lines + new_lines

        new_body = '\n'.join(all_lines)
        if not new_body.startswith('\n'):
            new_body = '\n' + new_body
        if not new_body.endswith('\n'):
            new_body = new_body + '\n'

        replacements.append((stub_start, stub_end, new_body))
        todos = sum(1 for l in new_lines if '// TODO' in l)
        if todos:
            print(f'  [{path.name}::{class_name}] {todos} remaining TODOs after resolve')

    if not replacements:
        return False

    replacements.sort(key=lambda x: x[0], reverse=True)
    for start, end, new in replacements:
        text = text[:start] + new + text[end:]

    if text != original:
        path.write_text(text, encoding='utf-8')
        return True
    return False

def main():
    all_h = [p for p in ROOT.rglob("*.h")
             if 'build-deps' not in p.parts and 'build-' not in str(p)]
    changed = []
    for p in all_h:
        try:
            if process_file(p):
                rel = p.relative_to(ROOT)
                changed.append(str(rel))
                print(f'  FIXED: {rel}')
        except Exception as e:
            print(f'  ERR {p.name}: {e}')
    print(f'\nDone. {len(changed)} files fixed.')

if __name__ == '__main__':
    main()
