#pragma once
#include "character.h"

class Plantune :public Character
{
public:
    //�R���X�g���N�^
    Plantune();
    //�f�X�g���N�^
    ~Plantune();

    void init()     override;
    void deinit()   override;
    void update()   override;
    void render()   override;
    void debug_imgui()    override;
};