#include "chat.h"


void our_chat_function(char* userinput, char* playerObject) {
    /*
    our chat function allows a player to enter commands that perform simple game hacks
    list of commands can be found in README
    */

    std::string input = userinput;
    struct Vec3 enemyPos = { *(get_player_x_addy()) + 400 , *(get_player_y_addy()), *(get_player_z_addy()) + 200 };

    //teleport player to an arena
    if (input.substr(0, 5) == "arena") {
        //if user types arena and doesn't enter a number, go to cow island by default
        if (input.length() == 5) {
            CreateArenaCowIsland();
            teleport(&Constants::cow_arena_coords);
        }
        else {
            int arenaChosen;
            sscanf_s(userinput + 6, "%d", &arenaChosen);
            switch (arenaChosen) {
            case 1: //vault arena
                CreateArenaVault();
                teleport(&Constants::vault_arena_coords);
                break;
            case 2: //under bridge arena
                CreateArenaUnderBridge();
                teleport(&Constants::bridge_arena_coords);
                break;
            }
        }
    }
    //teleport command 
    if (input.substr(0, 8) == "teleport") {
        float x; float y; float z;
        Vec3 tp_pos;
        sscanf_s(userinput + 9, "%f %f %f", &x, &y, &z);
        tp_pos = { x, y, z };
        teleport(&tp_pos);
    }

    //change player health
    if (input.substr(0, 6) == "health") {
        int health;
        sscanf_s(userinput + 7, "%d", &health);
        *(get_player_health_addy()) = health;
    }

    //change player mana
    if (input.substr(0, 4) == "mana") {
        int mana;
        sscanf_s(userinput + 5, "%d", &mana);
        *(get_player_mana_addy()) = mana;
    }
    //change player walking speed
    if (input.substr(0, 4) == "move") {
        float speed;
        sscanf_s(userinput + 5, "%f", &speed);
        *(get_player_walking_speed_addy()) = speed;
    }
#   //change player jump speed
    if (input.substr(0, 4) == "jump") {
        float speed;
        sscanf_s(userinput + 5, "%f", &speed);
        *(get_player_jump_speed_addy()) = speed;
    }
    //change player jump hold time
    if (input.substr(0, 4) == "time") {
        float time;
        sscanf_s(userinput + 5, "%f", &time);
        *(get_player_jump_hold_time_addy()) = time;
    }

    //add weapon to inventory
    if (input == "fireball") {
        GiveItem(Constants::fireball);
    }
    else if (input == "cube") {
        GiveItem(Constants::cube);
    }
    else if (input == "zerocool") {
        GiveItem(Constants::zero_cool);
    }
    else if (input == "akrifle") {
        GiveItem(Constants::ak_rifle);
    }
    else if (input == "ropchaingun") {
        GiveItem(Constants::rop_chain_gun);
    }
    else if (input == "goldenmaster") {
        GiveItem(Constants::golden_master);
    }
    else if (input == "heapspray") {
        GiveItem(Constants::heap_spray);
    }
    else if (input == "remotexploit") {
        GiveItem(Constants::remote_exploit);
    }
    else if (input == "cowboycoder") {
        GiveItem(Constants::cowboy_coder);
    }
    else if (input == "holyhandgrenade") {
        GiveItem(Constants::holy_hand_grenade);
    }

    //add ammo to inventory
    if (input == "ammo") {
        for (int i = 0; i < 50; i++) {
            GiveItem(Constants::pistol_ammo);
            GiveItem(Constants::rifle_ammo);
            GiveItem(Constants::shotgun_ammo);
            GiveItem(Constants::sniper_ammo);
            GiveItem(Constants::revolver_ammo);
        }
    }

    //spawn enemies
    if (input == "rat") {
        Actor* enemy_spawned = SpawnEnemy(GIANT_RAT_OFFSET, enemyPos);
    }
    else if (input == "spider") {
        Actor* enemy_spawned = SpawnEnemy(CAVE_SPIDER_OFFSET, enemyPos);
    }
    else if (input == "spider queen") {
        Actor* enemy_spawned = SpawnEnemy(ICE_SPIDER_Q_OFFSET, enemyPos);
    }
    else if (input == "bear") {
        Actor* enemy_spawned = SpawnEnemy(BEAR_OFFSET, enemyPos);
    }
    else if (input == "angry bear") {
        Actor* enemy_spawned = SpawnEnemy(ANGRY_BEAR_OFFSET, enemyPos);
    }
    else if (input == "mad cow") {
        Actor* enemy_spawned = SpawnEnemy(MAD_COW_OFFSET, enemyPos);
    }
    else if (input == "cow king") {
        Actor* enemy_spawned = SpawnEnemy(COW_K_OFFSET, enemyPos);
    }
    else if (input == "big boi") {
        Actor* enemy_spawned = SpawnEnemy(MAGMAROK_OFFSET, enemyPos);
    }
}

//this function calls our chat function and after it is has executed returns the state of the registers/stack
__declspec(naked) void function_inject_into_chat() {
    _asm {
        //push the arguments the chat function was called with onto the top of the stack 
        //pointer to player object is in ecx register
        push ecx

        //pointer to user input can be found at esp + 0x8 on the stack
        mov eax, [esp + 0x8]
        push eax

        //call our chat function
        call our_chat_function

        //now our function has executed we need to remove the values we pushed onto the stack
        pop eax
        pop ecx

        //execute the instructions overwritten in the chat function
        push ebp
        mov ebp, esp
        and esp, 0x0FFFFFFF8
        push 0x0FFFFFFFF

        //jump to the next instruction to be executed in the chat function
        jmp instruction
    }
}


void set_up_hook() {
    /*
    set_up_hook() will rewrite the 1st 5 bytes of the chat function with a jump instruction to function_inject_into_chat()
    */


    //address of chat function
    uintptr_t originalChatFunction = GameLogicDll + 0x551a0;

    //declare a pointer to the function we want to jumo to
    void* overwriteFunction = &function_inject_into_chat;

    //find the offset to the function we jump to
    DWORD sourceAddress = (DWORD)originalChatFunction + 5;
    DWORD destinationAddress = (DWORD)overwriteFunction;
    DWORD* offset = (DWORD*)(destinationAddress - sourceAddress);

    //create our jump instruction patch
    CHAR memoryPatch[5];
    memcpy(memoryPatch, "\xE9", 1); //jump instruction
    memcpy(memoryPatch + 1, &offset, 4); //offset to function_inject_into_chat()

    //patch the chat function with the jump instruction
    WriteProcessMemory(GetCurrentProcess(), (LPVOID)originalChatFunction, memoryPatch, 5, nullptr);

}