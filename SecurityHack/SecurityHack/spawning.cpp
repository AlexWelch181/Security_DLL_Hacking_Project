#include "spawning.h"

#include <iostream>
#include <math.h>
#include <Windows.h>
#include "constants.h"

uint32_t entityId = 10000;
uintptr_t rat_vtable[4096];
uintptr_t cow_vtable[4096];
uintptr_t bear_vtable[4096];
uintptr_t spider_vtable[4096];
uintptr_t spider_queen_vtable[4096];
uintptr_t angry_bear_vtable[4096];
uintptr_t cow_king_vtable[4096];
uintptr_t magmarock_vtable[4096];

// for use in vtable swapping - custom names and healths for entities
const char* get_rat_name() { return "Remy the rat"; }
const char* get_cow_name() { return "Milka"; }
const char* get_bear_name() { return "Halsin"; }
const char* get_angry_bear_name() { return "Angy Bear"; }

int get_damage_difficult() { return 500; }
int get_damage_medium() { return 250; }
int get_damage_easy() { return 50; }

bool arena_vault_created = false;
bool arena_bridge_created = false;
bool arena_cow_island_created = false;


void WorldSpawnActorWithId(World* world, int id, Actor* actor, struct Vec3* pos, struct Vec3* rot) {
    /*
    Calls the internal game function World::SpawnActorWithId
    */

    uintptr_t spawn_fn = GameLogicDll + 0x630c0;  // Pointer to function World::SpawnActorWithId

    _asm {
        // to prevent corrupting the registers - store all values to the stack (to be restored later)
        pushad
        // function parameters (in reverse order)
        mov eax, rot
        push eax
        mov eax, pos
        push eax
        mov eax, actor
        push eax
        mov eax, id
        push eax
        // ecx register is used to store the `this` pointer
        mov ecx, world
        // function pointer, to be called
        mov eax, spawn_fn
        call eax
        // restore previous register values
        popad
    }
}


void set_enemy_health(Actor* actor, int newHealth) {
    /*
    Function to update the health of an actor to a given value
    */

    int* mem_address = (int*)((uintptr_t)actor + 0x30);  // health variable is 0x30 offset from the start of the actor

    DWORD oldProtect;
    VirtualProtect(mem_address, sizeof(int), PAGE_READWRITE, &oldProtect);  // Remove write protection on the actors health address (Give us read & write access)

    *mem_address = newHealth;  // Lets replace the actor's health with our custom value
    
    VirtualProtect(mem_address, sizeof(int), oldProtect, &oldProtect);  // Set permissions back to how they were previously (write protection)
}


void set_enemy_name(Actor* actor, int mode, uintptr_t* hacked_vtable) {
    /*
    Function to change the display name of the enemy, by overriding the vtable
    */

    const char* (*func_tbl[])() = { &get_rat_name, &get_cow_name, &get_bear_name, &get_angry_bear_name };
    
    memcpy(hacked_vtable, *(uintptr_t**)(actor), 64 * sizeof(uintptr_t));  // copy the previous vtable into a variable
    
    hacked_vtable[20] = (uintptr_t)func_tbl[mode];  // update the name of the actor in the vtable
    
    *(uintptr_t*)actor = (uintptr_t)hacked_vtable;  // update the actor to point to the new vtable
}

void set_enemy_damage(Actor* actor, int mode, uintptr_t* hacked_vtable) {
    /*
    Function to change the damage dealt by an enemy, by overriding the vtable
    */

    int (*func_tbl[])() = { &get_damage_easy, &get_damage_medium, &get_damage_difficult };
    
    memcpy(hacked_vtable, *(void**)(actor), 64 * sizeof(uintptr_t));  // copy the previous vtable into a variable
    
    hacked_vtable[53] = (uintptr_t)func_tbl[mode];  // update the damage of the actor in the vtable

    *(uintptr_t*)actor = (uintptr_t)hacked_vtable;  // update the actor to point to the new vtable
}


uintptr_t AllocateActor(uintptr_t initialiser, int size=sizeof(Actor)) {
    /*
    Allocate and initialise an actor
    `initialiser` is a pointer to the constructor
    */

    uintptr_t buffer = (uintptr_t) malloc(size);  // Allocates heap memory for a new actor
    if (!buffer) { // malloc failed: reattempt
        Sleep(1000);
        return AllocateActor(initialiser, size);
    }

    _asm {
        // save previous register state
        pushad
        // ecx register is used to store the `this` pointer
        mov ecx, buffer
        mov eax, initialiser
        // call the constructor
        call eax
        // restore previous register state
        popad
    }

    // Return the pointer to our newly initialized Actor
    return buffer;
}


Actor * SummonEntity(struct Vec3 pos, struct Vec3 rot, long actor_initialiser_offset) {
    /*
    Initializes and then spawns Actor
    `pos` is the position of the new Actor
    `rot` is the rotation of the new Actor
    `actor_initialiser_offset` is the offset of the Actor constructor compared to GameLogic.dll
    */

    World* world = (World*)(GameLogicDll + WORLD_OFFSET);

    // initialise the entity
    Actor* actor = CreateEntity(actor_initialiser_offset);

    // spawn the entity
    WorldSpawnActorWithId(world, entityId++, actor, &pos, &rot);
    return actor;
}


void GiveItem(const char* itemName) {
    /*
    Gives the player an item with a specific name
    */

    uintptr_t getItemFunction = (uintptr_t)(GameLogicDll + 0x1DE20);
    uintptr_t addItemFunction = (uintptr_t)(GameLogicDll + 0x51C20);
    uintptr_t playerPtr = FollowPointerPath2(GameLogicDll + WORLD_OFFSET, { 0x1C, 0x6C, -0x70 });
    uintptr_t (*getGameAPI)() = (uintptr_t (*)())(GameLogicDll + 0x20CA0);

    uintptr_t item = 0x0;
    uintptr_t gameApi = (*getGameAPI)();

    // lookup the item from the name
    _asm {
        pushad

        // argument
        mov eax, itemName
        push eax
        // this
        mov ecx, gameApi
        // call
        mov eax, getItemFunction
        call eax
        mov item, eax

        popad
    }

    if (item != 0x0) {
        // give the player that item
        _asm {
            pushad

            // arguments
            mov eax, 1
            push eax
            mov eax, 1
            push eax
            mov eax, item
            push eax
            // this
            mov ecx, playerPtr
            // call
            mov eax, addItemFunction
            call eax

            popad
        }
    }
}


Actor* CreateEntity(long initialiser_offset) {
    /*
    Allocates and initialises an actor with a given initialiser offset
    */

    World* world = (World*)(GameLogicDll + WORLD_OFFSET);

    unsigned int size;  // in-memory size of the object
    switch (initialiser_offset) {
        case BUSH_OFFSET:
        case COW_CHEST_OFFSET:
            size = 0x70u;  // as we spawn many bushes and chests, it is important we do not waste additional memory while spawning them
            break;
        case MAGMAROK_OFFSET:
            size = 0x1000u;  // the magmarok has some weird behaviour with entity spawning - as such, we provide it with more memory
            break;
        default:
            size = 0x100u;
            break;
    }

    // create the memory space required
    uintptr_t actor = AllocateActor(GameLogicDll + initialiser_offset, size);

    return (Actor*)actor;
}


Actor* SpawnEnemy(int entity_offset, struct Vec3 pos, int nameMode, int health, int damage, uintptr_t* hacked_vtable) {
    /*
    An extension of SummonEntity that also applies a hacked vtable if provided
    */

    World* world = (World*)(GameLogicDll + WORLD_OFFSET);
    
    struct Vec3 rot = { 0.0f, 0.0f, 0.0f };
    Actor* actor = CreateEntity(entity_offset);
    
    // only apply vtable hacks if one is provided
    if (hacked_vtable) {
        if (nameMode != -1) { set_enemy_name(actor, nameMode, hacked_vtable); }
        if (damage != -1) { set_enemy_damage(actor, damage, hacked_vtable); }
    }
    // health is not stored in the vtable, so we can always apply it
    if (health != -1) { set_enemy_health(actor, health); }

    // spawn the enemy
    WorldSpawnActorWithId(world, entityId++, actor, &pos, &rot);
    return actor;
}


void CreateArenaVault() {
    /*
    Creates the arena in the vault if it doesn't already exist
    */

    if (!arena_vault_created) {
        MakeCircle(Constants::vault_arena_coords, 1500, COW_CHEST_OFFSET, 250, 200, 2);
        arena_vault_created = true;
    }
}

void CreateArenaUnderBridge() {
    /*
    Creates the arena under the bridge if it doesn't already exist
    */

    if (!arena_bridge_created) {
        MakeCircle(Constants::bridge_arena_coords, 4000, BUSH_OFFSET, 360, 200, 1);
        arena_bridge_created = true;
    }
}

void CreateArenaCowIsland() {
    /*
    Creates the arena on cow island if it doesn't already exist
    */

    if (!arena_cow_island_created) {
        MakeCircle(Constants::cow_arena_coords, 2500, COW_CHEST_OFFSET, 250, 200, 2);
        arena_cow_island_created = true;
    }
}


void MakeCircle(struct Vec3 centre, float radius, long actor_initialiser_offset, int actor_width, int actor_height, int circle_height) { 
    /*
    Spawns a circle of actors
    `centre` is the position that is the center of the circle
    `radius` is the radius of the circle
    `actor_initialiser_offset` is the offset of the Actor constructor compared to GameLogic.dll
    `actor_width` is the approx width of the actor object (so we know how many to put in a circle)
    `actor_height` is the approx height of the actor object (so we know how to stack them)
    `circle_height` is how many actors to stack
    */
   
    // calculate the number of actors to spawn - proportional to 
    float circumference = 3.141569265f * 2 * radius;
    int num_actors = (int) floor(circumference / actor_width);

    // used to point all actors towards the centre of the circle
    struct Vec3 rot = { 0.0f, 0.0f, 0.0f };

    for (int i = 0; i < num_actors; i++) {
        float angle = (3.141569265f * 2 * i) / (float)num_actors;
        struct Vec3 pos = { 0, 0, 0 };

        // rotate the actor towards the centre
        rot.y = -angle * 360 / (3.141569265f * 2);

        // position the actor correctly
        pos.x = centre.x - (radius * sinf(angle));
        pos.y = centre.y - (radius * cosf(angle));
        pos.z = centre.z;
        
        // optional: spawn multiple tiers of entity
        for (int i = 0; i < circle_height; i++) {
            pos.z += i * actor_height;
            SummonEntity(pos, rot, actor_initialiser_offset);
        }
    }
}