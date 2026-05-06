#ifndef ESTAMPTOOL_H
#define ESTAMPTOOL_H

#include <functional>
#include <vector>

enum class eBuildingType;
enum class eCityId;
enum class ePlayerId;

struct eStampElement {
    eBuildingType type;
    int dx, dy;
};

class eStampTool {
public:
    eStampTool();

    const std::vector<eStampElement>& blueprint() const { return mBlueprint; }
    std::vector<eStampElement> transformedBlueprint() const;
    void setRotation(int r) { mRotation = r % 4; }
    int rotation() const { return mRotation; }
    void setMirror(int m) { mMirror = m % 2; }
    int mirror() const { return mMirror; }
    using eDrawXY  = std::function<void(int tx, int ty, double& rx, double& ry, int sw, int sh, int alt)>;
    using eDrawTex = std::function<void(double rx, double ry, eBuildingType type, int sw, bool canBuild)>;

    void paintPreview(int baseX, int baseY, class eGameBoard* board,
                      bool editorMode, eCityId viewedCityId, ePlayerId playerId,
                      const eDrawXY& drawXY, const eDrawTex& drawTex) const;

    bool canBuildAt(int baseX, int baseY, class eGameBoard* board,
                    bool editorMode, eCityId viewedCityId, ePlayerId playerId) const;
    void buildAt(int baseX, int baseY, class eGameBoard* board, eCityId viewedCityId, ePlayerId playerId, bool editorMode);

private:
    static void buildingSize(eBuildingType type, int& sw, int& sh);
    static void gameAnchorOffset(eBuildingType type, int& dx, int& dy);

    std::vector<eStampElement> mBlueprint;
    int mRotation = 0;
    int mMirror = 0;
};

#endif // ESTAMPTOOL_H
