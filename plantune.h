#pragma once
#include "character.h"

class Plantune :public Character
{
public:
    //コンストラクタ
    Plantune();
    //デストラクタ
    ~Plantune();

    void init()     override;
    void deinit()   override;
    void update()   override;
    void render()   override;
    void debug_imgui()    override;
};