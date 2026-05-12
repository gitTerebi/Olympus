#!/usr/bin/env python3
"""Add serializeJson implementations to building cpp files."""

import re, sys

PATCHES = {
    "buildings/eresourcecollectbuilding.cpp": {
        "include": '#include "fileIO/esavearchive.h"',
        "after_serialize": "void eResourceCollectBuilding::serialize(eSaveArchive& ar)",
        "impl": '''
void eResourceCollectBuilding::serializeJson(eJsonArchive& ar) {
    eResourceCollectBuildingBase::serializeJson(ar);
    ar.field("mCollectedAction", mCollectedAction);
    ar.field("mSpawnEnabled", mSpawnEnabled);
    ar.field("mAddResource", mAddResource);
    ar.field("mRawCount", mRawCount);
    ar.field("mRawCountCollect", mRawCountCollect);
    ar.field("mRawInc", mRawInc);
    ar.field("mProcessDuration", mProcessDuration);
    ar.field("mProcessTime", mProcessTime);
    ar.field("mWaitTime", mWaitTime);
    ar.field("mSpawnTime", mSpawnTime);
    // mCollector restored by character load pass
}
'''
    },
    "buildings/estoragebuilding.cpp": {
        "include": '#include "fileIO/esavearchive.h"',
        "after_serialize": "void eStorageBuilding::serialize(eSaveArchive& ar)",
        "impl": '''
void eStorageBuilding::serializeJson(eJsonArchive& ar) {
    eEmployingBuilding::serializeJson(ar);
    ar.field("mGet", mGet);
    ar.field("mEmpty", mEmpty);
    ar.field("mAccept", mAccept);
    for(int i = 0; i < 15; i++) {
        const auto k = "mResourceCount." + std::to_string(i);
        ar.field(k.c_str(), mResourceCount[i]);
    }
    for(int i = 0; i < 15; i++) {
        const auto k = "mResource." + std::to_string(i);
        ar.field(k.c_str(), mResource[i]);
    }
    int nc = 0;
    if(ar.writing()) nc = static_cast<int>(mMaxCount.size());
    ar.field("nc", nc);
    if(ar.reading()) mMaxCount.clear();
    auto it = mMaxCount.begin();
    for(int i = 0; i < nc; i++) {
        eResourceType rt{};
        int c = 0;
        if(ar.writing()) { rt = it->first; c = it->second; ++it; }
        const auto rk = "rt." + std::to_string(i);
        const auto ck = "c." + std::to_string(i);
        ar.field(rk.c_str(), rt);
        ar.field(ck.c_str(), c);
        if(ar.reading()) mMaxCount[rt] = c;
    }
    // mCart1/mCart2 restored by character load pass
}
'''
    },
    "buildings/epatrolbuildingbase.cpp": {
        "include": '#include "fileIO/esavearchive.h"',
        "after_serialize": "void ePatrolBuildingBase::serialize(eSaveArchive& ar)",
        "impl": '''
void ePatrolBuildingBase::serializeJson(eJsonArchive& ar) {
    eEmployingBuilding::serializeJson(ar);
    ar.field("mBothDirections", mBothDirections);
    ar.field("mLastDirection", mLastDirection);
    ar.field("mSpawnPatrolers", mSpawnPatrolers);
    ar.field("mSpawnTime", mSpawnTime);
    ar.field("mSpawnRoadId", mSpawnRoadId);
    int n = 0;
    if(ar.writing()) n = static_cast<int>(mPatrolGuides.size());
    ar.field("n", n);
    if(ar.reading()) mPatrolGuides.clear();
    for(int i = 0; i < n; i++) {
        ePatrolGuide pg;
        if(ar.writing()) pg = mPatrolGuides[i];
        const auto xk = "pg.fX." + std::to_string(i);
        const auto yk = "pg.fY." + std::to_string(i);
        ar.field(xk.c_str(), pg.fX);
        ar.field(yk.c_str(), pg.fY);
        if(ar.reading()) mPatrolGuides.push_back(pg);
    }
    // mDirTimes/mChar restored by character load pass
}
'''
    },
    "buildings/epatrolsourcebuilding.cpp": {
        "include": '#include "fileIO/esavearchive.h"',
        "after_serialize": "void ePatrolSourceBuilding::serialize(eSaveArchive& ar)",
        "impl": '''
void ePatrolSourceBuilding::serializeJson(eJsonArchive& ar) {
    ePatrolBuilding::serializeJson(ar);
    for(int i = 0; i < static_cast<int>(mTargetData.size()); i++) {
        auto& tt = mTargetData[i];
        const auto stk = "tt.fSpawnTime." + std::to_string(i);
        const auto lik = "tt.fLastId." + std::to_string(i);
        ar.field(stk.c_str(), tt.fSpawnTime);
        ar.field(lik.c_str(), tt.fLastId);
    }
}
'''
    },
    "buildings/eagorabase.cpp": {
        "include": '#include "fileIO/esavearchive.h"',
        "after_serialize": "void eAgoraBase::serialize(eSaveArchive& ar)",
        "impl": '''
void eAgoraBase::serializeJson(eJsonArchive& ar) {
    ePatrolBuildingBase::serializeJson(ar);
    setMaxEmployees(0);
    fillSpaces();
}
'''
    },
    "buildings/evendor.cpp": {
        "include": '#include "fileIO/esavearchive.h"',
        "after_serialize": "void eVendor::serialize(eSaveArchive& ar)",
        "impl": '''
void eVendor::serializeJson(eJsonArchive& ar) {
    eEmployingBuilding::serializeJson(ar);
    ar.field("mResource", mResource);
    ar.field("mVendorEnabled", mVendorEnabled);
    // mCart restored by character load pass
    // agora ref set by reader postFunc via "agora" key
}
'''
    },
    "buildings/emonument.cpp": {
        "include": '#include "fileIO/esavearchive.h"',
        "after_serialize": "void eMonument::serialize(eSaveArchive& ar)",
        "impl": '''
void eMonument::serializeJson(eJsonArchive& ar) {
    eEmployingBuilding::serializeJson(ar);
    ar.field("mRotated", mRotated);
    ar.field("mHaltConstruction", mHaltConstruction);
    ar.field("mStored.fWood", mStored.fWood);
    ar.field("mStored.fMarble", mStored.fMarble);
    ar.field("mStored.fSculpture", mStored.fSculpture);
    ar.field("mUsed.fWood", mUsed.fWood);
    ar.field("mUsed.fMarble", mUsed.fMarble);
    ar.field("mUsed.fSculpture", mUsed.fSculpture);
    ar.field("mAltitude", mAltitude);
    // mCart restored by character load pass
}
'''
    },
    "buildings/etradepost.cpp": {
        "include": '#include "fileIO/esavearchive.h"',
        "after_serialize": "void eTradePost::serialize(eSaveArchive& ar)",
        "impl": '''
void eTradePost::serializeJson(eJsonArchive& ar) {
    eStorageBuilding::serializeJson(ar);
    ar.field("mImports", mImports, eResourceType::none);
    ar.field("mExports", mExports, eResourceType::none);
    ar.field("mCartEmpty", mCartEmpty, eResourceType::none);
    ar.field("mCartGet", mCartGet, eResourceType::none);
    ar.field("mCartAccept", mCartAccept, eResourceType::none);
    ar.field("mRouteTimer", mRouteTimer, 0);
}
'''
    },
    "buildings/ehousebase.cpp": {
        "include": '#include "fileIO/esavearchive.h"',
        "after_serialize": "void eHouseBase::serialize(eSaveArchive& ar)",
        "impl": '''
void eHouseBase::serializeJson(eJsonArchive& ar) {
    eBuilding::serializeJson(ar);
    ar.field("mLevel", mLevel);
    ar.field("mPeople", mPeople);
    ar.field("mFood", mFood);
    ar.field("mFleece", mFleece);
    ar.field("mOil", mOil);
    ar.field("mPhilosophers", mPhilosophers);
    ar.field("mActors", mActors);
    ar.field("mAthletes", mAthletes);
    ar.field("mCompetitors", mCompetitors);
    ar.field("mPaidTaxes", mPaidTaxes);
    ar.field("mPaidTaxesLastMonth", mPaidTaxesLastMonth);
}
'''
    },
    "buildings/esmallhouse.cpp": {
        "include": '#include "fileIO/esavearchive.h"',
        "after_serialize": None,  # no serialize method, add at end
        "impl": '''
void eSmallHouse::serializeJson(eJsonArchive& ar) {
    eHouseBase::serializeJson(ar);
    ar.field("mWater", mWater);
    ar.field("mHygiene", mHygiene);
    ar.field("mPlague", mPlague);
    ar.field("mDisgruntled", mDisgruntled);
    ar.field("mSatisfaction", mSatisfaction);
    ar.field("mFoodSatisfaction", mFoodSatisfaction);
    ar.field("mWaterSatisfaction", mWaterSatisfaction);
    ar.field("mWorkSatisfaction", mWorkSatisfaction);
    ar.field("mTaxSatisfaction", mTaxSatisfaction);
    ar.field("mDevolveDelay", mDevolveDelay);
    ar.field("mEvictDelay", mEvictDelay);
}
'''
    },
    "buildings/epatroltarget.cpp": {
        "include": '#include "fileIO/esavearchive.h"',
        "after_serialize": "void ePatrolTarget::serialize(eSaveArchive& ar)",
        "impl": '''
void ePatrolTarget::serializeJson(eJsonArchive& ar) {
    ePatrolBuilding::serializeJson(ar);
    ar.field("mAvailable", mAvailable);
}
'''
    },
    "buildings/eprocessingbuilding.cpp": {
        "include": '#include "fileIO/esavearchive.h"',
        "after_serialize": "void eProcessingBuilding::serialize(eSaveArchive& ar)",
        "impl": '''
void eProcessingBuilding::serializeJson(eJsonArchive& ar) {
    eResourceBuildingBase::serializeJson(ar);
    ar.field("mSpawnTime", mSpawnTime);
    ar.field("mProducedThisYear", mProducedThisYear);
    for(int i = 0; i < 12; i++) {
        const auto k = "mMonthlyProduced." + std::to_string(i);
        ar.field(k.c_str(), mMonthlyProduced[i]);
    }
    ar.field("mRingIdx", mRingIdx);
    ar.field("mRawCount", mRawCount);
    ar.field("mProcessWaitTime", mProcessWaitTime);
}
'''
    },
    "buildings/eshepherbuildingbase.cpp": {
        "include": '#include "fileIO/esavearchive.h"',
        "after_serialize": "void eShepherBuildingBase::serialize(eSaveArchive& ar)",
        "impl": '''
void eShepherBuildingBase::serializeJson(eJsonArchive& ar) {
    eResourceBuildingBase::serializeJson(ar);
    ar.field("mSpawnTime", mSpawnTime);
    ar.field("mProducedThisYear", mProducedThisYear);
    for(int i = 0; i < 12; i++) {
        const auto k = "mMonthlyProduced." + std::to_string(i);
        ar.field(k.c_str(), mMonthlyProduced[i]);
    }
    ar.field("mRingIdx", mRingIdx);
    // mShepherd restored by character load pass
}
'''
    },
    "buildings/ehuntinglodge.cpp": {
        "include": '#include "fileIO/esavearchive.h"',
        "after_serialize": "void eHuntingLodge::serialize(eSaveArchive& ar)",
        "impl": '''
void eHuntingLodge::serializeJson(eJsonArchive& ar) {
    eResourceCollectBuildingBase::serializeJson(ar);
    ar.field("mSpawnTime", mSpawnTime);
    // mHunter restored by character load pass
}
'''
    },
    "buildings/etriremewharf.cpp": {
        "include": '#include "fileIO/esavearchive.h"',
        "after_serialize": "void eTriremeWharf::serialize(eSaveArchive& ar)",
        "impl": '''
void eTriremeWharf::serializeJson(eJsonArchive& ar) {
    eEmployingBuilding::serializeJson(ar);
    ar.field("mAbroad", mAbroad);
    ar.field("mTriremeBuildingStage", mTriremeBuildingStage);
    ar.field("mTriremeBuildingTime", mTriremeBuildingTime);
    // mTakeCart/mTrireme restored by character load pass
}
'''
    },
    "buildings/eurchinquay.cpp": {
        "include": '#include "fileIO/esavearchive.h"',
        "after_serialize": "void eUrchinQuay::serialize(eSaveArchive& ar)",
        "impl": '''
void eUrchinQuay::serializeJson(eJsonArchive& ar) {
    eResourceCollectBuildingBase::serializeJson(ar);
    ar.field("mDisabled", mDisabled);
    ar.field("mStateCount", mStateCount);
    ar.field("mState", mState);
    // mGatherer restored by character load pass
}
'''
    },
    "buildings/etower.cpp": {
        "include": '#include "fileIO/esavearchive.h"',
        "after_serialize": "void eTower::serialize(eSaveArchive& ar)",
        "impl": '''
void eTower::serializeJson(eJsonArchive& ar) {
    eEmployingBuilding::serializeJson(ar);
    ar.field("mMissile", mMissile);
    ar.field("mRangeAttack", mRangeAttack);
    ar.field("mAttackTime", mAttackTime);
    ar.field("mAttack", mAttack);
    ar.field("mAttackOrientation", mAttackOrientation);
    ar.field("mSpawnTime", mSpawnTime);
    // mAttackTarget/mArcher restored by character load pass
}
'''
    },
    "buildings/epalace.cpp": {
        "include": '#include "fileIO/esavearchive.h"',
        "after_serialize": "void ePalace::serialize(eSaveArchive& ar)",
        "impl": '''
void ePalace::serializeJson(eJsonArchive& ar) {
    eEmployingBuilding::serializeJson(ar);
    // mTiles linked via buildingRef in reader/writer
}
'''
    },
    "buildings/eruins.cpp": {
        "include": '#include "fileIO/esavearchive.h"',
        "after_serialize": "void eRuins::serialize(eSaveArchive& ar)",
        "impl": '''
void eRuins::serializeJson(eJsonArchive& ar) {
    eBuilding::serializeJson(ar);
    ar.field("mWasType", mWasType);
    ar.field("mOriginX", mOriginX);
    ar.field("mOriginY", mOriginY);
    ar.field("mOriginW", mOriginW);
    ar.field("mOriginH", mOriginH);
}
'''
    },
    "buildings/sanctuaries/esanctbuilding.cpp": {
        "include": '#include "fileIO/esavearchive.h"',
        "after_serialize": "void eSanctBuilding::serialize(eSaveArchive& ar)",
        "impl": '''
void eSanctBuilding::serializeJson(eJsonArchive& ar) {
    eEmployingBuilding::serializeJson(ar);
    ar.field("mWorkedOn", mWorkedOn);
    ar.field("mProgress", mProgress);
    ar.field("mHalted", mHalted);
}
'''
    },
    "buildings/sanctuaries/esanctuary.cpp": {
        "include": '#include "fileIO/esavearchive.h"',
        "after_serialize": "void eSanctuary::serialize(eSaveArchive& ar)",
        "impl": '''
void eSanctuary::serializeJson(eJsonArchive& ar) {
    eMonument::serializeJson(ar);
    auto& board = getBoard();
    int nw = 0;
    if(ar.writing()) nw = static_cast<int>(mWarriorTiles.size());
    ar.field("nw", nw);
    if(ar.reading()) mWarriorTiles.clear();
    for(int i = 0; i < nw; i++) {
        eTile* t = ar.writing() ? mWarriorTiles[i] : nullptr;
        const auto k = "wt." + std::to_string(i);
        ar.tile(k.c_str(), t, board);
        if(ar.reading() && t) mWarriorTiles.push_back(t);
    }
    int ns = 0;
    if(ar.writing()) ns = static_cast<int>(mSpecialTiles.size());
    ar.field("ns", ns);
    if(ar.reading()) mSpecialTiles.clear();
    for(int i = 0; i < ns; i++) {
        eTile* t = ar.writing() ? mSpecialTiles[i] : nullptr;
        const auto k = "st." + std::to_string(i);
        ar.tile(k.c_str(), t, board);
        if(ar.reading() && t) mSpecialTiles.push_back(t);
    }
    // mGod/mSoldierBanners not saved in JSON path
}
'''
    },
    "buildings/sanctuaries/etemplebuilding.cpp": {
        "include": '#include "fileIO/esavearchive.h"',
        "after_serialize": "void eTempleBuilding::serialize(eSaveArchive& ar)",
        "impl": '''
void eTempleBuilding::serializeJson(eJsonArchive& ar) {
    eSanctBuilding::serializeJson(ar);
    ar.field("mId", mId);
}
'''
    },
    "buildings/sanctuaries/etemplealtarbuilding.cpp": {
        "include": '#include "fileIO/esavearchive.h"',
        "after_serialize": "void eTempleAltarBuilding::serialize(eSaveArchive& ar)",
        "impl": '''
void eTempleAltarBuilding::serializeJson(eJsonArchive& ar) {
    eSanctBuilding::serializeJson(ar);
    ar.field("mSacrifice", mSacrifice);
    ar.field("mSacrificeTime", mSacrificeTime);
}
'''
    },
    "buildings/sanctuaries/eartemissanctuary.cpp": {
        "include": '#include "fileIO/esavearchive.h"',
        "after_serialize": "void eSanctuaryWithWarriors::serialize(eSaveArchive& ar)",
        "impl": '''
void eSanctuaryWithWarriors::serializeJson(eJsonArchive& ar) {
    eSanctuary::serializeJson(ar);
    ar.field("mSoldierSpawn", mSoldierSpawn);
    // mSoldierBanners not saved in JSON path
}
'''
    },
    "buildings/pyramids/epyramidelement.cpp": {
        "include": '#include "fileIO/esavearchive.h"',
        "after_serialize": "void ePyramidElement::serialize(eSaveArchive& ar)",
        "impl": '''
void ePyramidElement::serializeJson(eJsonArchive& ar) {
    eBuilding::serializeJson(ar);
    ar.field("mCurrentElevation", mCurrentElevation);
}
'''
    },
    "buildings/pyramids/epyramidbuildingpart.cpp": {
        "include": '#include "fileIO/esavearchive.h"',
        "after_serialize": "void ePyramidBuildingPart::serialize(eSaveArchive& ar)",
        "impl": '''
void ePyramidBuildingPart::serializeJson(eJsonArchive& ar) {
    eBuilding::serializeJson(ar);
    ar.field("mPaintDir", mPaintDir);
    // mPaint cross-ref not saved in JSON path
}
'''
    },
    "buildings/pyramids/epyramid.cpp": {
        "include": '#include "fileIO/esavearchive.h"',
        "after_serialize": "void ePyramid::serialize(eSaveArchive& ar)",
        "impl": '''
void ePyramid::serializeJson(eJsonArchive& ar) {
    eMonument::serializeJson(ar);
    int ds = 0;
    if(ar.writing()) ds = static_cast<int>(mDark.size());
    ar.field("ds", ds);
    if(ar.reading()) mDark.clear();
    for(int i = 0; i < ds; i++) {
        bool d = ar.writing() ? mDark[i] : false;
        const auto k = "d." + std::to_string(i);
        ar.field(k.c_str(), d);
        if(ar.reading()) mDark.push_back(d);
    }
}
'''
    },
}

def process_file(path, patch):
    with open(path, 'r', encoding='utf-8') as f:
        content = f.read()

    # Add include if not present
    if 'ejsonarchive.h' not in content:
        content = content.replace(patch['include'],
                                  patch['include'] + '\n#include "fileIO/ejsonarchive.h"')

    # Find insertion point: end of the closing } of the serialize method
    after = patch.get('after_serialize')
    if after:
        idx = content.find(after)
        if idx == -1:
            print(f"WARNING: could not find '{after}' in {path}", file=sys.stderr)
            return
        # Find the closing brace of that function
        brace_start = content.find('{', idx)
        depth = 0
        pos = brace_start
        while pos < len(content):
            if content[pos] == '{': depth += 1
            elif content[pos] == '}':
                depth -= 1
                if depth == 0:
                    break
            pos += 1
        insert_at = pos + 1
    else:
        # Insert at end of file
        insert_at = len(content)

    content = content[:insert_at] + '\n' + patch['impl'] + content[insert_at:]
    with open(path, 'w', encoding='utf-8') as f:
        f.write(content)
    print(f"Patched {path}")

import os
os.chdir(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

for rel_path, patch in PATCHES.items():
    process_file(rel_path, patch)

print("Done.")
