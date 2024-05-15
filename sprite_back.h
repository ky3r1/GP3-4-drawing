#pragma once
#include "character.h"

class Sprite_Back : public Character
{
public:
    //コンストラクタ
    Sprite_Back();
    //デストラクタ
    ~Sprite_Back();

    void init()     override;
    void deinit()   override;
    void update()   override;
    void render()   override;
    void debug_imgui() override;
};