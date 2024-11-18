#pragma once

#include "libGameLogic.h"
#include "pointers.h"

// offsets to the world pointer from the dll injection point
#ifndef WORLD_OFFSET
#define WORLD_OFFSET 0x97D7C
#endif

// offsets to chest initialisers
#define BUSH_OFFSET 0x142A0
#define BEAR_CHEST_OFFSET 0x71E0
#define COW_CHEST_OFFSET 0xD540
#define LAVA_CHEST_OFFSET 0x3AFE0
#define BLOCKY_CHEST_OFFSET 0x8CD0

// offsets to mob initialisers
#define GIANT_RAT_OFFSET 0x383C0
#define CAVE_SPIDER_OFFSET 0x9000
#define ICE_SPIDER_Q_OFFSET 0x39730
#define BEAR_OFFSET 0x6010
#define ANGRY_BEAR_OFFSET 0x49E0
#define MAD_COW_OFFSET 0x3C720
#define COW_K_OFFSET 0xD870
#define MAGMAROK_OFFSET 0x3CBC0

void WorldSpawnActorWithId(World* world, int id, Actor* actor, struct Vec3* pos, struct Vec3* rot);
Actor* SummonEntity(struct Vec3 pos, struct Vec3 rot, long actor_initialiser_offset);
Actor* CreateEntity(long initialiser_offset);
Actor* SpawnEnemy(int entity_offset, struct Vec3 pos, int nameMode = -1, int health = -1, int damage = -1, uintptr_t* hacked_vtable = nullptr);
void CreateArenaVault();
void CreateArenaUnderBridge();
void CreateArenaCowIsland();
void MakeCircle(struct Vec3 centre, float radius, long actor_initialiser_offset, int actor_width, int actor_height, int circle_height);
void GiveItem(const char* itemName);