#ifndef ESTAMPTOOL_H
#define ESTAMPTOOL_H

#include <functional>
#include <string>
#include <utility>
#include <vector>

#include "widgets/ebuildingmode.h"

enum class eBuildingType;
enum class eCityId;
enum class eDifficulty;
enum class ePlayerId;

struct eStampElement {
    eBuildingType type;
    int dx, dy;
    int id = -1;
};

struct eStampBuildCommand {
    eBuildingMode mode;
    int dx, dy;
    int agoraOrientation = -1;
    std::vector<std::pair<int, int>> agoraRoads;
    std::vector<std::pair<int, int>> agoraSpaces;
    bool agoraRect = false;
};

class eStampTool {
public:
    eStampTool();

    const std::vector<eStampElement>& blueprint() const { return mBlueprint; }
    const std::string& templateName() const { return mTemplateName; }
    bool setTemplate(const std::string& name, const std::string& path);
    std::vector<eStampElement> transformedBlueprint() const;
    std::vector<eStampBuildCommand> buildCommands() const;
    int estimatedCost(eDifficulty diff) const;
    void setRotation(int r) { mRotation = r % 4; }
    int rotation() const { return mRotation; }
    void setMirror(int m) { mMirror = m % 2; }
    int mirror() const { return mMirror; }
    using eDrawXY  = std::function<void(int tx, int ty, double& rx, double& ry, int sw, int sh, int alt)>;
    using eDrawTex = std::function<void(double rx, double ry, eBuildingType type, int sw, bool canBuild)>;
    using eDrawAgora = std::function<void(int tx, int ty, int id)>;

    void paintPreview(int baseX, int baseY, class eGameBoard* board,
                      bool editorMode, eCityId viewedCityId, ePlayerId playerId,
                      const eDrawXY& drawXY, const eDrawTex& drawTex,
                      const eDrawAgora& drawAgora) const;

private:
    static void buildingSize(eBuildingType type, int& sw, int& sh);
    static void elementSize(const eStampElement& elem, int& sw, int& sh);
    static void buildingDrawOffset(int sw, int sh, int& dx, int& dy);
    void transformPoint(int& dx, int& dy) const;
    eStampElement transformedElement(const eStampElement& elem) const;

    std::vector<eStampElement> mBlueprint;
    std::string mTemplateName = "Default";
    int mRotation = 0;
    int mMirror = 0;
};

#endif // ESTAMPTOOL_H
