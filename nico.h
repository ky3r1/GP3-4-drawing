#pragma once
#include "character.h"

class Nico : public Character
{
public:
    //�R���X�g���N�^
    Nico();
    //�f�X�g���N�^
    ~Nico();

    void init()     override;
    void deinit()   override;
    void update()   override;
    void render()   override;
    void debug_imgui()    override;
};