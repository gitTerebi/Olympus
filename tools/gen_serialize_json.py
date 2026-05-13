#!/usr/bin/env python3
"""
Codemod v3: fix (void)ar stub serializeJson bodies.
When read() body is in .cpp (declared only in .h), searches the .cpp.
Also handles serialize(eSaveArchive&) pattern in .cpp.
"""

import re
from pathlib import Path

ROOT = Path(r"C:\Users\somtam\Downloads\_repo\eZeus")

def strip_const_cast(s):
    return re.sub(r'const_cast<[^>]+>\(([^)]+)\)', r'\1', s)

def find_brace_end(text, start):
    assert text[start] == '{', f"Expected '{{' at {start}, got {text[start]!r}"
    depth = 1
    p = start + 1
    while p < len(text) and depth > 0:
        if text[p] == '{': depth += 1
        elif text[p] == '}': depth -= 1
        p += 1
    return p

def extract_top_stmts(body):
    stmts = []
    cur = []
    depth = 0
    for c in body.strip():
        if c == '{':
            depth += 1
            cur.append(c)
        elif c == '}':
            depth -= 1
            cur.append(c)
        elif c == ';' and depth == 0:
            s = ''.join(cur).strip()
            if s:
                stmts.append(s)
            cur = []
        else:
            cur.append(c)
    s = ''.join(cur).strip()
    if s:
        stmts.append(s)
    return stmts

def parse_lambda_assign(lambda_body, param_name):
    body = re.sub(r'\s+', ' ', lambda_body).strip().lstrip('{').rstrip('}').strip().rstrip(';')
    m = re.match(r'(m\w+)\s*=\s*(.*)', body)
    if not m:
        return None, None
    member = m.group(1)
    rhs = m.group(2).strip()
    cm = re.match(r'(static_cast<[^>]+>)\s*\(\s*' + re.escape(param_name) + r'\s*\)', rhs)
    if cm:
        return member, cm.group(1)
    return member, None

def translate_stmt(s, out_lines, seen_bases):
    s = re.sub(r'\s+', ' ', s).strip()
    if not s:
        return True

    # base::read(src) or base::serialize(ar)
    m = re.match(r'([\w:]+)::(read|serialize)\s*\(', s)
    if m:
        base = m.group(1)
        if base not in seen_bases:
            out_lines.append(f'        {base}::serializeJson(ar);')
            seen_bases.add(base)
        return True

    # eSaveArchive ar(...) constructor
    if re.match(r'eSaveArchive\s+\w+\s*\(', s):
        return True

    # ar.field / archive.field
    m = re.match(r'\w+\.field\s*\(\s*"([^"]+)"\s*,\s*(.+)\)', s)
    if m:
        key, val = m.group(1), strip_const_cast(m.group(2)).strip()
        out_lines.append(f'        ar.field("{key}", {val});')
        return True

    # src.readBuilding
    m = re.match(
        r'src\.readBuilding\s*\(&board\(\)\s*,\s*\[this\]\s*\(eBuilding\s*\*\s*(?:const\s+)?(\w+)\)\s*(\{[^}]*\})', s)
    if m:
        param, lb = m.group(1), m.group(2)
        member, cast = parse_lambda_assign(lb, param)
        if member:
            out_lines.append(f'        eBuilding* _{member} = {member}.get();')
            out_lines.append(f'        ar.buildingRef("{member}", _{member}, board());')
            cast_str = f'{cast}(_{member})' if cast else f'_{member}'
            out_lines.append(f'        if(ar.reading()) {member} = {cast_str};')
            return True

    # src.readCharacter
    m = re.match(
        r'src\.readCharacter\s*\(&board\(\)\s*,\s*\[this\]\s*\(eCharacter\s*\*\s*(?:const\s+)?(\w+)\)\s*(\{[^}]*\})', s)
    if m:
        param, lb = m.group(1), m.group(2)
        member, cast = parse_lambda_assign(lb, param)
        if member:
            out_lines.append(f'        eCharacter* _{member} = {member}.get();')
            out_lines.append(f'        ar.characterRef("{member}", _{member}, board());')
            cast_str = f'{cast}(_{member})' if cast else f'_{member}'
            out_lines.append(f'        if(ar.reading()) {member} = {cast_str};')
            return True

    # src.readCharacterAction
    m = re.match(
        r'src\.readCharacterAction\s*\(&board\(\)\s*,\s*\[this\]\s*\(eCharacterAction\s*\*\s*(?:const\s+)?(\w+)\)\s*(\{[^}]*\})', s)
    if m:
        param, lb = m.group(1), m.group(2)
        member, cast = parse_lambda_assign(lb, param)
        if member:
            resolve = f'{cast}(board().characterActionWithIOID(_ioid))' if cast else 'board().characterActionWithIOID(_ioid)'
            out_lines += [
                f'        if(ar.writing()) {{',
                f'            int _ioid = {member} ? {member}->ioID() : -1;',
                f'            ar.field("{member}", _ioid);',
                f'        }} else {{',
                f'            int _ioid = -1;',
                f'            ar.field("{member}", _ioid);',
                f'            if(_ioid >= 0) ar.addPostFunc([this, _ioid]() {{ {member} = {resolve}; }});',
                f'        }}',
            ]
            return True

    # mX = src.readTile(board())
    m = re.match(r'(m\w+)\s*=\s*src\.readTile\s*\(board\(\)\)', s)
    if m:
        out_lines.append(f'        ar.tile("{m.group(1)}", {m.group(1)}, board());')
        return True

    # mX = src.readGodAct / ar.readStream().readGodAct
    m = re.match(r'(m\w+)\s*=\s*(?:ar\.readStream\(\)\.)?readGodAct\s*\(', s)
    if m:
        out_lines.append(f'        ar.godActRef("{m.group(1)}", {m.group(1)}, board());')
        return True

    # src.readBuilding with callback (alternative: no &board())
    m = re.match(
        r'src\.readBuilding\s*\(\w+,\s*\[this\]\s*\(eBuilding\s*\*\s*(?:const\s+)?(\w+)\)\s*(\{[^}]*\})', s)
    if m:
        param, lb = m.group(1), m.group(2)
        member, cast = parse_lambda_assign(lb, param)
        if member:
            out_lines.append(f'        eBuilding* _{member} = {member}.get();')
            out_lines.append(f'        ar.buildingRef("{member}", _{member}, board());')
            cast_str = f'{cast}(_{member})' if cast else f'_{member}'
            out_lines.append(f'        if(ar.reading()) {member} = {cast_str};')
            return True

    return False

def generate_body(read_body):
    flat = re.sub(r'\s+', ' ', read_body).strip()
    stmts = extract_top_stmts(flat)
    out, unrecog = [], 0
    seen_bases = set()
    for s in stmts:
        if not translate_stmt(s, out, seen_bases):
            out.append(f'        // TODO: {s[:100]}')
            unrecog += 1
    return out, unrecog

def find_method_in_text(text, class_name, method_re_str):
    """Find method body either as inline or as ClassName::method in text."""
    # inline
    m = re.search(method_re_str, text)
    if m:
        ob = text.find('{', m.end() - 1)
        if ob >= 0:
            end = find_brace_end(text, ob)
            return text[ob+1:end-1]
    # qualified: ClassName::read(
    qre = re.escape(class_name) + r'\s*::\s*(?:read|serialize)\s*\([^)]*\)\s*\{'
    m = re.search(qre, text)
    if m:
        ob = text.rfind('{', m.start(), m.end())
        if ob >= 0:
            end = find_brace_end(text, ob)
            return text[ob+1:end-1]
    return None

def get_cpp_text(h_path):
    cpp = h_path.with_suffix('.cpp')
    if cpp.exists():
        return cpp.read_text(encoding='utf-8', errors='replace')
    return ''

def process_file(path):
    text = path.read_text(encoding='utf-8', errors='replace')
    if '(void)ar;' not in text:
        return False

    original = text
    cpp_text = get_cpp_text(path)

    stub_re = re.compile(
        r'(void\s+serializeJson\s*\(\s*eJsonArchive\s*&\s*\w+\s*\)'
        r'(?:\s*override)?\s*\{)\s*\(void\)ar;\s*\}',
        re.MULTILINE
    )

    replacements = []
    for sm in stub_re.finditer(text):
        stub_start, stub_end = sm.start(), sm.end()
        preceding = text[:stub_start]

        # find class name by scanning backwards for 'class X'
        class_m = None
        for cm in re.finditer(r'\bclass\s+(\w+)', preceding):
            class_m = cm
        class_name = class_m.group(1) if class_m else ''

        # try to find read body: first inline in preceding text
        read_body = None

        # look for inline read() just before stub
        inline_re = r'\bvoid\s+read\s*\(\s*eReadStream\s*&\s*\w*\s*\)(?:\s*override)?\s*\{'
        inline_m = None
        for im in re.finditer(inline_re, preceding):
            inline_m = im
        if inline_m:
            ob = preceding.find('{', inline_m.end() - 1)
            if ob >= 0:
                end = find_brace_end(preceding, ob)
                read_body = preceding[ob+1:end-1]

        # if no inline, look in cpp
        if not read_body and cpp_text and class_name:
            # try ClassName::read(
            qre = re.escape(class_name) + r'\s*::\s*read\s*\([^)]*\)\s*\{'
            m = re.search(qre, cpp_text)
            if m:
                ob = cpp_text.rfind('{', m.start(), m.end() + 1)
                if ob < 0:
                    ob = cpp_text.find('{', m.end() - 1)
                if ob >= 0:
                    end = find_brace_end(cpp_text, ob)
                    read_body = cpp_text[ob+1:end-1]

            # try ClassName::serialize(eSaveArchive
            if not read_body:
                qre2 = re.escape(class_name) + r'\s*::\s*serialize\s*\(\s*eSaveArchive'
                m = re.search(qre2, cpp_text)
                if m:
                    ob = cpp_text.find('{', m.end())
                    if ob >= 0:
                        end = find_brace_end(cpp_text, ob)
                        read_body = cpp_text[ob+1:end-1]

        if not read_body:
            continue

        body_lines, unrecog = generate_body(read_body)
        if not body_lines:
            body_lines = ['        // no fields']

        indent_m = re.search(r'\n(\s*)void\s+serializeJson', text[:stub_start])
        indent = indent_m.group(1) if indent_m else '    '

        new_body = (
            'void serializeJson(eJsonArchive& ar) override {\n'
            + '\n'.join(body_lines) + '\n'
            + f'{indent}}}'
        )
        replacements.append((stub_start, stub_end, new_body))
        if unrecog:
            print(f'  [{path.name}::{class_name}] {unrecog} TODO stmts')

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
    all_h = list(ROOT.rglob("*.h"))
    # exclude build-deps
    all_h = [p for p in all_h if 'build-deps' not in p.parts and 'build-' not in str(p)]
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
