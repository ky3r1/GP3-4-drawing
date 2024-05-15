#pragma once
#include "character.h"

class Nico : public Character
{
public:
    //コンストラクタ
    Nico();
    //デストラクタ
    ~Nico();

    void init()     override;
    void deinit()   override;
    void update()   override;
    void render()   override;
    void debug_imgui()    override;
};