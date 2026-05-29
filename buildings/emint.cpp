#include "emint.h"

#include "characters/silver-miner.h"
#include "textures/egametextures.h"

eMint::eMint(GameBoard& board, const eCityId cid) :
    eResourceCollectBuilding(board,
                             &eBuildingTextures::fMint,
                             -3.73, -3.73,
                             &eBuildingTextures::fMintOverlay,
                             3, 0.5, -1.5,
                             [this]() { return e::make_shared<SilverMiner>(getBoard()); },
                             eBuildingType::mint,
                             eHasResourceObject::sCreate(eHasResourceObjectType::silver),
                             2, 2, 15, eResourceType::silver, cid) {
    eGameTextures::loadMint();
    setRawCountCollect(4);
}
