#include "pointers.h"

uintptr_t FollowPointerPath(uintptr_t pointer, std::vector<unsigned int> offsets)
    /*
    Function to follow a list of pointer paths to an address

    From base pointer, 'pointer'
    Using the list of offsets, 'offsets'
    */
    {
        uintptr_t addr = pointer;
        for (unsigned int i = 0; i < offsets.size(); ++i) {
            addr = *(uintptr_t*)addr;
            addr += offsets[i];
        }

        return addr;
    }

uintptr_t FollowPointerPath2(uintptr_t pointer, std::vector<int> offsets) {  // todo: refactor to remove v2
    /*
    Shorthand for following a pointer path - takes a list of offsets and follows them, similar to Cheat Engine
    */

    uintptr_t addr = pointer;
    for (unsigned int i = 0; i < offsets.size(); ++i) {
        addr = *(uintptr_t*)addr; // dereference
        addr += offsets[i];  // offset
    }

    return addr;
}



int* get_player_health_addy() {
    /*
    Returns integer pointer to the players health
    */

    return (int*)(FollowPointerPath(GameLogicDll + WORLD_OFFSET, { 0x1C, 0x6C, 0xffffffc0 }));
}

int* get_player_mana_addy() {
    /*
    Returns integer pointer to the players mana
    */

    return (int*)(FollowPointerPath(GameLogicDll + WORLD_OFFSET, { 0x1C, 0x6C, 0xBC }));
}

float* get_player_walking_speed_addy() {
    /*
    Returns floating point pointer to the players max walking speed
    */

    return (float*)(FollowPointerPath(GameLogicDll + WORLD_OFFSET, { 0x1C, 0x6C, 0x120 }));
}

float* get_player_jump_speed_addy() {
    /*
    Returns floating point pointer to the players jump speed
    */

    return (float*)(FollowPointerPath(GameLogicDll + WORLD_OFFSET, { 0x1C, 0x6C, 0x124 }));
}

float* get_player_jump_hold_time_addy() {
    /*
    Returns floating point pointer to the players jump hold time (hold space for longer jump up to hold time)
    */

    return (float*)(FollowPointerPath(GameLogicDll + WORLD_OFFSET, { 0x1C, 0x6C, 0x128 }));
}


float* get_player_x_addy() {
    /*
    Returns floating point pointer to the players x coordinate
    */

    return (float*)(FollowPointerPath(PwnAdventAddr + 0x18FCD60, { 0x20, 0x238, 0x280, 0x90 }));
}

float* get_player_y_addy() {
    /*
    Returns floating point pointer to the players y coordinate
    */

    return (float*)(FollowPointerPath(PwnAdventAddr + 0x18FCD60, { 0x20, 0x238, 0x280, 0x94 }));
}


float* get_player_z_addy() {
    /*
    Returns floating point pointer to the players z coordinate
    */

    return (float*)(FollowPointerPath(PwnAdventAddr + 0x18FCD60, { 0x20, 0x238, 0x280, 0x98 }));
}

void teleport(Vec3* pos) {
    /*
    Teleports the player to x, y, z
    */
    *(get_player_x_addy()) = pos->x;
    *(get_player_y_addy()) = pos->y;
    *(get_player_z_addy()) = pos->z;
    printf("teleported to %f, %f, %f", pos->x, pos->y, pos->z);
}


// Initialize global variables
uintptr_t GameLogicDll = (uintptr_t)GetModuleHandle(L"GameLogic.dll");
uintptr_t PwnAdventAddr = (uintptr_t)GetModuleHandle(L"PwnAdventure3-Win32-Shipping.exe");
uintptr_t instruction = GameLogicDll + 0x551a8;