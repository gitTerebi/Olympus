#ifndef EBLOB_H
#define EBLOB_H

#include <functional>
#include <vector>
#include <string>
#include "fileIO/ewritestream.h"
#include "fileIO/ereadstream.h"

static const char kB64Tbl[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

inline std::string b64enc(const std::vector<char>& d) {
    std::string o;
    o.reserve((d.size() + 2) / 3 * 4);
    for(size_t i = 0; i < d.size(); i += 3) {
        unsigned char a = (unsigned char)d[i];
        unsigned char b = i+1 < d.size() ? (unsigned char)d[i+1] : 0;
        unsigned char c = i+2 < d.size() ? (unsigned char)d[i+2] : 0;
        o += kB64Tbl[a >> 2];
        o += kB64Tbl[((a&3)<<4)|(b>>4)];
        o += i+1 < d.size() ? kB64Tbl[((b&0xf)<<2)|(c>>6)] : '=';
        o += i+2 < d.size() ? kB64Tbl[c&0x3f]              : '=';
    }
    return o;
}

inline std::vector<char> b64dec(const std::string& s) {
    static unsigned char t[256] = {};
    static bool init = false;
    if(!init) { for(int i = 0; i < 64; i++) t[(unsigned char)kB64Tbl[i]] = i; init = true; }
    std::vector<char> o;
    o.reserve(s.size() / 4 * 3);
    for(size_t i = 0; i + 3 < s.size(); i += 4) {
        unsigned char a = t[(unsigned char)s[i]],   b = t[(unsigned char)s[i+1]];
        unsigned char c = t[(unsigned char)s[i+2]], d = t[(unsigned char)s[i+3]];
        o.push_back((char)((a<<2)|(b>>4)));
        if(s[i+2] != '=') o.push_back((char)(((b&0xf)<<4)|(c>>2)));
        if(s[i+3] != '=') o.push_back((char)(((c&3)<<6)|d));
    }
    return o;
}

inline std::string captureWrite(const std::function<void(eWriteStream&)>& fn) {
    std::vector<char> buf;
    eWriteTarget tgt(&buf);
    eWriteStream dst(tgt);
    dst.writeFormat("eZeus.blob");
    fn(dst);
    return b64enc(buf);
}

inline void replayRead(const std::string& b64, const std::function<void(eReadStream&)>& fn) {
    if(b64.empty()) return;
    auto buf = b64dec(b64);
    eReadSource src(buf.data());
    eReadStream s(src);
    s.readFormat();
    fn(s);
    s.handlePostFuncs();
}

#endif // EBLOB_H
