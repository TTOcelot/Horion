#include "AutoClicker.h"

AutoClicker::AutoClicker() : IModule(0, Category::COMBAT, "A simple autoclicker, automatically clicks for you.") {
    this->registerBoolSetting("rightclick", &this->rightclick, rightclick);
    this->registerBoolSetting("only weapons", &this->weapons, this->weapons);
    this->registerBoolSetting("break blocks", &this->breakBlocks, this->breakBlocks);
    this->registerIntSetting("delay", &this->delay, this->delay, 1, 30); // Ajustamos el rango de delay para permitir hasta 30 cps
    this->registerBoolSetting("hold", &this->hold, this->hold);
}

AutoClicker::~AutoClicker() {
}

const char* AutoClicker::getModuleName() {
    return ("AutoClicker");
}

void AutoClicker::onTick(C_GameMode* gm) {
    static float clickTimer = 0; // Acumulador fraccional

    if ((GameData::isLeftClickDown() || !hold) && GameData::canUseMoveKeys()) {
        C_LocalPlayer* localPlayer = g_Data.getLocalPlayer();
        PointingStruct* pointing = g_Data.getClientInstance()->getPointerStruct();
        clickTimer += (1.0f / 20.0f); // Incrementamos el temporizador fraccional en cada tick

        if (clickTimer >= (1.0f / delay)) { // Comparamos con el tiempo calculado
            auto selectedItem = localPlayer->getSelectedItem();
            if (weapons && selectedItem->getAttackingDamageWithEnchants() < 1)
                return;

            g_Data.leftclickCount++;

            localPlayer->swingArm();

            if (pointing->hasEntity() != 0)
                gm->attack(pointing->getEntity());
            else if (breakBlocks) {
                bool isDestroyed = false;
                gm->startDestroyBlock(pointing->block, pointing->blockSide, isDestroyed);
                gm->stopDestroyBlock(pointing->block);
                if (isDestroyed && localPlayer->region->getBlock(pointing->block)->blockLegacy->blockId != 0)
                    gm->destroyBlock(&pointing->block, 0);
            }
            clickTimer = 0; // Reseteamos el temporizador fraccional
        }
    }

    if (rightclick) {
        if ((GameData::isRightClickDown() || !hold) && GameData::canUseMoveKeys()) {
            PointingStruct* pstruct = g_Data.getClientInstance()->getPointerStruct();
            clickTimer += (1.0f / 20.0f); // Incrementamos el temporizador fraccional en cada tick

            if (clickTimer >= (1.0f / delay)) { // Comparamos con el tiempo calculado
                g_Data.rightclickCount++;
                gm->buildBlock(new vec3_ti(pstruct->block), pstruct->blockSide);
                clickTimer = 0; // Reseteamos el temporizador fraccional
            }
        }
    }
}
