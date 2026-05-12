#ifndef EJSONARCHIVE_H
#define EJSONARCHIVE_H

#include <string>
#include <fstream>
#include <functional>
#include <type_traits>
#include <cstdint>
#include <vector>
#include <memory>
#include "json.hpp"

using njson = nlohmann::json;

class eGameBoard;
class eTile;
class eBuilding;

class eJsonArchive {
public:
    explicit eJsonArchive(njson& root)
        : mRoot(&root), mReading(false),
          mPostFuncs(std::make_shared<std::vector<PostFunc>>()) {}
    explicit eJsonArchive(const njson& root)
        : mRootConst(&root), mReading(true),
          mPostFuncs(std::make_shared<std::vector<PostFunc>>()) {}

    bool reading() const { return mReading; }
    bool writing() const { return !mReading; }

    // scalar: bool, float, double, std::string
    bool field(const char* path, bool&        v, bool        def = false)  { return fImpl(path, v, def); }
    bool field(const char* path, float&       v, float       def = 0.f)   { return fImpl(path, v, def); }
    bool field(const char* path, double&      v, double      def = 0.0)   { return fImpl(path, v, def); }
    bool field(const char* path, std::string& v, const std::string& def = {}) { return fImpl(path, v, def); }

    // all integral types (int, int32_t, uint16_t, etc.) — stored as int64
    template <typename T,
              typename = std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T,bool>>>
    bool field(const char* path, T& v, T def = T{}) {
        if(mReading) {
            const njson* n = get(path);
            if(!n || n->is_null()) { v = def; return false; }
            try { v = static_cast<T>(n->get<int64_t>()); } catch(...) { v = def; return false; }
            return true;
        }
        set(path) = static_cast<int64_t>(v);
        return true;
    }

    // floating point
    template <typename T,
              typename = std::enable_if_t<std::is_floating_point_v<T>>,
              typename = void, typename = void>
    bool field(const char* path, T& v, T def = T{}) {
        if(mReading) {
            const njson* n = get(path);
            if(!n || n->is_null()) { v = def; return false; }
            try { v = static_cast<T>(n->get<double>()); } catch(...) { v = def; return false; }
            return true;
        }
        set(path) = static_cast<double>(v);
        return true;
    }

    // enum — stored as int32
    template <typename T,
              typename = std::enable_if_t<std::is_enum_v<T>>,
              typename = void>
    bool field(const char* path, T& v, T def = T{}) {
        int32_t i = static_cast<int32_t>(v);
        int32_t d = static_cast<int32_t>(def);
        bool ok = field(path, i, d);
        v = static_cast<T>(i);
        return ok;
    }

    // loop helper — handles count + indexed body in one call
    // usage:  ar.loop("things", n, [&](int i){ ar.field(...); });
    void loop(const char* countKey, int& n,
              const std::function<void(int)>& body) {
        field(countKey, n);
        for(int i = 0; i < n; i++) body(i);
    }

    // sub-archive for a named child node (dot-path aware)
    eJsonArchive child(const char* key) {
        if(mReading) {
            const njson* n = get(key);
            static const njson sNull;
            return eJsonArchive(n ? *n : sNull, mPostFuncs);
        }
        return eJsonArchive(set(key), mPostFuncs);
    }
    // sub-archive for an indexed array element (dot-path aware)
    eJsonArchive childAt(const char* key, int i) {
        if(mReading) {
            const njson* n = get(key);
            static const njson sNull;
            if(!n || !n->is_array() || i < 0 || i >= static_cast<int>(n->size()))
                return eJsonArchive(sNull, mPostFuncs);
            return eJsonArchive((*n)[i], mPostFuncs);
        }
        njson& arr = set(key);
        if(!arr.is_array()) arr = njson::array();
        while(static_cast<int>(arr.size()) <= i) arr.push_back(njson::object());
        return eJsonArchive(arr[i], mPostFuncs);
    }

    // tile: stored as {valid, x, y}
    void tile(const char* key, eTile*& t, eGameBoard& board);

    // building cross-reference: stored as IOID int; on read, deferred via postFunc
    void buildingRef(const char* key, eBuilding*& b, eGameBoard& board);
    void buildingRef(const char* key, const std::function<void(eBuilding*)>& cb, eGameBoard& board);

    // deferred callbacks (analogous to eReadStream::addPostFunc)
    using PostFunc = std::function<void()>;
    void addPostFunc(PostFunc f) { mPostFuncs->push_back(std::move(f)); }
    using IndexCb = std::function<void(int)>;
    void runPostFuncs(IndexCb before = {}, IndexCb after = {}) {
        int idx = 0;
        for(auto& f : *mPostFuncs) {
            if(before) before(idx);
            f();
            if(after) after(idx);
            idx++;
        }
        mPostFuncs->clear();
    }
    size_t postFuncCount() const { return mPostFuncs ? mPostFuncs->size() : 0; }

    // static I/O
    static bool saveToFile(const njson& root, const std::string& path) {
        std::ofstream f(path);
        if(!f) return false;
        f << root.dump(2);
        return f.good();
    }
    static bool loadFromFile(const std::string& path, njson& root) {
        std::ifstream f(path);
        if(!f) return false;
        try { f >> root; } catch(...) { return false; }
        return true;
    }

private:
    eJsonArchive(njson& root, std::shared_ptr<std::vector<PostFunc>> pf)
        : mRoot(&root), mReading(false), mPostFuncs(std::move(pf)) {}
    eJsonArchive(const njson& root, std::shared_ptr<std::vector<PostFunc>> pf)
        : mRootConst(&root), mReading(true), mPostFuncs(std::move(pf)) {}

    njson*       mRoot      = nullptr;
    const njson* mRootConst = nullptr;
    bool         mReading;
    std::shared_ptr<std::vector<PostFunc>> mPostFuncs;

    const njson* get(const char* path) const {
        const njson* n = mReading ? mRootConst : mRoot;
        const char* p = path;
        while(*p) {
            if(!n->is_object()) return nullptr;
            const char* dot = p;
            while(*dot && *dot != '.') ++dot;
            std::string key(p, dot);
            auto it = n->find(key);
            if(it == n->end()) return nullptr;
            n = &(*it);
            p = *dot ? dot + 1 : dot;
        }
        return n;
    }

    njson& set(const char* path) {
        njson* n = mRoot;
        const char* p = path;
        while(*p) {
            if(!n->is_object() && !n->is_null()) { *n = njson::object(); }
            const char* dot = p;
            while(*dot && *dot != '.') ++dot;
            std::string key(p, dot);
            n = &((*n)[key]);
            p = *dot ? dot + 1 : dot;
        }
        return *n;
    }

    template <typename T>
    bool fImpl(const char* path, T& v, const T& def) {
        if(mReading) {
            const njson* n = get(path);
            if(!n || n->is_null()) { v = def; return false; }
            try { v = n->get<T>(); } catch(...) { v = def; return false; }
            return true;
        }
        set(path) = v;
        return true;
    }
};

#endif // EJSONARCHIVE_H
