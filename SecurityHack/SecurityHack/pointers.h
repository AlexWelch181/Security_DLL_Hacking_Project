#pragma once

#include <cstdint>
#include <Windows.h>
#include <vector>

#ifndef WORLD_OFFSET
#define WORLD_OFFSET 0x97D7C
#endif

extern uintptr_t GameLogicDll;
extern uintptr_t PwnAdventAddr;
extern uintptr_t instruction;

int* get_player_health_addy();
int* get_player_mana_addy();
float* get_player_walking_speed_addy();
float* get_player_jump_speed_addy();
float* get_player_jump_hold_time_addy();

float* get_player_x_addy();
float* get_player_y_addy();
float* get_player_z_addy();

uintptr_t FollowPointerPath(uintptr_t pointer, std::vector<unsigned int> offsets);
uintptr_t FollowPointerPath2(uintptr_t pointer, std::vector<int> offsets);

// used for generic locations / rotations
struct Vec3 {
    float x;
    float y;
    float z;
};

void teleport(Vec3* pos);