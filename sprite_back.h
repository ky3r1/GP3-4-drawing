#pragma once
#include "character.h"

class Sprite_Back : public Character
{
public:
    //�R���X�g���N�^
    Sprite_Back();
    //�f�X�g���N�^
    ~Sprite_Back();

    void init()     override;
    void deinit()   override;
    void update()   override;
    void render()   override;
    void debug_imgui() override;
};