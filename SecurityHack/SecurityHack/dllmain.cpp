// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include <iostream>
#include <string>
#include <dxgi.h>
#include <d3d11.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/kiero.h"
#include <tchar.h>
#include <vector>
#include "libGameLogic.h"
#include "spawning.h"
#include "constants.h"
#include "pointers.h"
#include "chat.h"

bool show_window = false;

int arena_campaign_difficulty = 0;
int arena_location = 0;
int enemy_selected = 0;
int weapon_selection = 0;
int ammo_selection = 0;
int enemy_damage_selection = 0;
int enemy_health = 100;
int enemy_name = 0;

struct Vec3 arenaCoordinates = { 0, 0, 0 };

uintptr_t spawn_vtable[4096];

uintptr_t originalChatFunction;
static uint64_t hacked_vtable[4096];

bool setup = false;
bool gameOver = true;
//int difficulty = 1;
int round_count = 1;
Actor* enemies[10]{};
uintptr_t player_tick_func;
uintptr_t instruction2;


void spawn_enemies(Actor* enemies[], int numEnemies) {
    /*
    Spawns waves of enemies for arena: camapign mode
    */

    for (int i = 0; i < numEnemies; i++) {
        Actor* enemy_spawned;
        

        //sets random coorinates for enemy to spawn at from centre of arena  
        struct Vec3 spawnLocation = arenaCoordinates;
        float rand1 = rand() % 2000;
        float rand2 = rand() % 2000;
        printf("%f %f", rand1, rand2);
        spawnLocation.x = spawnLocation.x - float(1000) + rand1;
        spawnLocation.y = spawnLocation.y - float(1000) + rand2;
        spawnLocation.z = spawnLocation.z + float(100);


        //defines enemies health based on the difficulty 
        int enemyHealth;

        switch (arena_campaign_difficulty) {
        case 1:
            printf("easy?");
            enemyHealth = 50 + (5 * round_count);
            break;
        case 2:
            printf("medium?");
            enemyHealth = 250 + (5 * round_count);
            break;
        case 3:
            printf("hard?");
            enemyHealth = 500 + (5 * round_count);
            break;
        case 4:
            enemyHealth = 1000 + (5 * round_count);
            break;
        }

        //Enemy spawns for Vault
        if (arena_location == 0) {
            //sets number of enemy types based on the round
            int enemyType;
            if (round_count > 4) {
                enemyType = rand() % 3;
            }
            else {
                enemyType = rand() % 2;
            }
            //random value for type of enemy

            switch (enemyType) {
            case 0: // Giant Rat
                enemy_spawned = SpawnEnemy(GIANT_RAT_OFFSET, spawnLocation, -1, enemyHealth);
                enemies[i] = enemy_spawned;
                break;
            case 1: // Cave Spider
                enemy_spawned = SpawnEnemy(CAVE_SPIDER_OFFSET, spawnLocation, -1, enemyHealth);
                enemies[i] = enemy_spawned;
                break;
            case 2: //ice spider queen 
                enemy_spawned = SpawnEnemy(ICE_SPIDER_Q_OFFSET, spawnLocation, -1, enemyHealth);
                enemies[i] = enemy_spawned;
                break;
            }
        }

        //Enemy spawns for under bridge 
        if (arena_location == 1) {
            //sets number of enemy types based on the round
            int enemyType;
            if (round_count > 4) {
                enemyType = rand() % 2;
            }
            else {
                enemyType = 0;
            }
            //random value for type of enemy

            switch (enemyType) {
            case 0: // Angry Bear 
                enemy_spawned = SpawnEnemy(ANGRY_BEAR_OFFSET, spawnLocation, -1, enemyHealth);
                enemies[i] = enemy_spawned;
                break;
            case 1: // Magmarok
                enemy_spawned = SpawnEnemy(MAGMAROK_OFFSET, spawnLocation, -1, enemyHealth);
                enemies[i] = enemy_spawned;
                break;
            }
        }

        //Enemy spawns for Cow island 
        if (arena_location == 2) {
            //sets number of enemy types based on the round
            int enemyType;
            if (round_count > 6) {
                enemyType = rand() % 2;
            }
            else {
                enemyType = 0;
            }
            //random value for type of enemy
            //printf("enemytype = %d ", enemyType);

            switch (enemyType) {
            case 0: // Mad Cow
                enemy_spawned = SpawnEnemy(MAD_COW_OFFSET, spawnLocation, -1, enemyHealth);
                enemies[i] = enemy_spawned;
                break;
            case 1:  // Cow King
                enemy_spawned = SpawnEnemy(COW_K_OFFSET, spawnLocation, -1, enemyHealth);
                enemies[i] = enemy_spawned;
                break;
            }
        }
    }
}

void randomItem() {
    /*
    Function to give the player a random item and the relevant ammo for that item
    */

    int random = rand() % 7;
    printf("\n%d\n", random);
    GiveItem(Constants::weapons[random]);
    switch (random) {
    case 0:
        GiveItem(Constants::rifle_ammo);
        break;
    case 1:
        GiveItem(Constants::shotgun_ammo);
        break;
    case 3:
        GiveItem(Constants::shotgun_ammo);
        break;
    case 4:
        GiveItem(Constants::sniper_ammo);
        break;
    }
}


void gameLogic(char* player, float delta_time) {
    /*
    Main game logic for arena: campaign mode
    */

    if (gameOver) {
        printf("Game Over\n");
        return;
    }

    int numEnemies = 1 + (2 * (round_count - 1));
    if (numEnemies > 10) {
        numEnemies = 10;
    }

    if (!setup) {
        spawn_enemies(enemies, numEnemies);
        setup = true;
        printf("setup complete\n");
    }
    else {
        if (round_count > 100) {
            gameOver = true;
            return;
        }

        int count = 0;
        for (int i = 0; i < numEnemies; i++) {
            int* mem_address = (int*)((uintptr_t)enemies[i] + 0x30);
            if (*mem_address <= 0) {
                count++;
            }
        }
        if (count == numEnemies) {
            printf("enemies dead");
            //system("pause");
            //difficulty += 1;
            round_count += 1;
            setup = false;

            //decrease player health by 10 after each wave but keep at min of 50
            if (*(get_player_health_addy()) < 50) {
                *(get_player_health_addy()) = 50;
            }
            else {
                int playerHealth = *(get_player_health_addy());
                *(get_player_health_addy()) = playerHealth - 10;
            }

            //decrease player speed by 2 after each wave but keep at min of 50
            if (*(get_player_walking_speed_addy()) < 50) {
                *(get_player_walking_speed_addy()) = 50;
            }
            else {
                float walkingSpeed = *(get_player_walking_speed_addy());
                *(get_player_walking_speed_addy()) = walkingSpeed - 2;
            }
            randomItem(); //player given a new item at end of wave

        }
    }
}

__declspec(naked) void new_player_tick() {
    /*
    
    */

    _asm {
        mov eax, [esp + 0x4]
        push eax
        push ecx
        call gameLogic
        pop ecx
        pop eax
        push ebp
        mov ebp, esp
        and esp, 0x0FFFFFFC0
        push 0x0FFFFFFFF
        mov eax, instruction2
        jmp eax
    }
}

void overwrite_player_tick() {
    /*
    
    */

    printf("player tick being overwritten\n");
    player_tick_func = GameLogicDll + 0x50730;
    instruction2 = GameLogicDll + 0x50738;

    void* myFunc = &new_player_tick;

    DWORD src = (DWORD)player_tick_func + 5;
    DWORD dst = (DWORD)myFunc;
    DWORD* offset = (DWORD*)(dst - src);

    CHAR memPatch[5];

    memcpy(memPatch, "\xE9", 1); //jump instruction
    memcpy(memPatch + 1, &offset, 4); //offset to our chat function 

    DWORD oldProtect = 0;

    printf("GameLogicDll at %p\n", GameLogicDll);
    printf("Player tick at %p\n", player_tick_func);

    if (!VirtualProtectEx(GetCurrentProcess(), (LPVOID)player_tick_func, sizeof(memPatch), PAGE_EXECUTE_READWRITE, &oldProtect)) {
        printf("VirtualProtectEX error %lu\n", GetLastError());
    }
    if (!WriteProcessMemory(GetCurrentProcess(), (LPVOID)player_tick_func, memPatch, sizeof(memPatch), nullptr)) {
        printf("Write Process Memory Failed with error %lu\n", GetLastError());
    }
    VirtualProtectEx(GetCurrentProcess(), (LPVOID)player_tick_func, sizeof(memPatch), oldProtect, nullptr);
}


void player_settings_tab() {
    /*
    Player Settings tab within the hack menu
    */


    ImGui::SetCursorPos(ImVec2(45, 70));
    ImGui::Text("Player Health");

    ImGui::SetCursorPos(ImVec2(45, 125));
    ImGui::Text("Player Mana");

    ImGui::SetCursorPos(ImVec2(45, 235));
    ImGui::Text("Player X Coord");

    ImGui::SetCursorPos(ImVec2(45, 290));
    ImGui::Text("Player Y Coord");

    ImGui::SetCursorPos(ImVec2(45, 345));
    ImGui::Text("Player Z Coord");

    ImGui::SetCursorPos(ImVec2(45, 455));
    ImGui::Text("Player Walk Speed");

    ImGui::SetCursorPos(ImVec2(45, 510));
    ImGui::Text("Player Jump Speed");

    ImGui::SetCursorPos(ImVec2(45, 565));
    ImGui::Text("Player Jump Hold");

    ImGui::SetCursorPos(ImVec2(175, 70));
    ImGui::PushItemWidth(200);
    ImGui::InputInt("##Player_Health", get_player_health_addy());
    ImGui::PopItemWidth();

    ImGui::SetCursorPos(ImVec2(175, 125));
    ImGui::PushItemWidth(200);
    ImGui::InputInt("##Player_Mana", get_player_mana_addy());
    ImGui::PopItemWidth();

    ImGui::SetCursorPos(ImVec2(175, 235));
    ImGui::PushItemWidth(200);
    ImGui::InputFloat("##Player_X", get_player_x_addy());
    ImGui::PopItemWidth();

    ImGui::SetCursorPos(ImVec2(175, 290));
    ImGui::PushItemWidth(200);
    ImGui::InputFloat("##Player_Y", get_player_y_addy());
    ImGui::PopItemWidth();

    ImGui::SetCursorPos(ImVec2(175, 345));
    ImGui::PushItemWidth(200);
    ImGui::InputFloat("##Player_Z", get_player_z_addy());
    ImGui::PopItemWidth();

    ImGui::SetCursorPos(ImVec2(175, 455));
    ImGui::PushItemWidth(200);
    ImGui::InputFloat("##Player_Walk_Speed", get_player_walking_speed_addy());
    ImGui::PopItemWidth();

    ImGui::SetCursorPos(ImVec2(175, 510));
    ImGui::PushItemWidth(200);
    ImGui::InputFloat("##Player_Jump_Speed", get_player_jump_speed_addy());
    ImGui::PopItemWidth();

    ImGui::SetCursorPos(ImVec2(175, 565));
    ImGui::PushItemWidth(200);
    ImGui::InputFloat("##Player_Jump_Hold", get_player_jump_hold_time_addy());
    ImGui::PopItemWidth();
}

void player_items_tab() {
    /*
    Player Items tab within the hack menu
    */

    ImGui::SetCursorPos(ImVec2(120, 200));
    ImGui::PushItemWidth(200);
    ImGui::Combo("##WEAPON_SELECTION", &weapon_selection, Constants::weapon_selections, IM_ARRAYSIZE(Constants::weapon_selections));
    ImGui::PopItemWidth();

    ImGui::SetCursorPos(ImVec2(175, 255));
    if (ImGui::Button("Give Weapon")) {
        switch (weapon_selection) {
        case 0:
            GiveItem(Constants::fireball);
            break;
        case 1:
            GiveItem(Constants::zero_cool);
            break;
        case 2:
            GiveItem(Constants::ak_rifle);
            break;
        case 3:
            GiveItem(Constants::rop_chain_gun);
            break;
        case 4:
            GiveItem(Constants::golden_master);
            break;
        case 5:
            GiveItem(Constants::heap_spray);
            break;
        case 6:
            GiveItem(Constants::remote_exploit);
            break;
        case 7:
            GiveItem(Constants::cowboy_coder);
            break;
        case 8:
            GiveItem(Constants::holy_hand_grenade);
            break;
        case 9:
            GiveItem(Constants::cube);
            break;
        }
    }

    ImGui::SetCursorPos(ImVec2(120, 365));
    ImGui::PushItemWidth(200);
    ImGui::Combo("##AMMO_SELECTION", &ammo_selection, Constants::ammo_selections, IM_ARRAYSIZE(Constants::ammo_selections));
    ImGui::PopItemWidth();
    ImGui::SetCursorPos(ImVec2(175, 420));
    if (ImGui::Button("Give Ammo")) {
        switch (ammo_selection) {
        case 0:
            GiveItem(Constants::pistol_ammo);
            break;
        case 1:
            GiveItem(Constants::rifle_ammo);
            break;
        case 2:
            GiveItem(Constants::shotgun_ammo);
            break;
        case 3:
            GiveItem(Constants::sniper_ammo);
            break;
        }
    }
}

void arena_sandbox_tab() {
    /*
    Arena Sandbox Tab within the hack menu
    */


    ImGui::SetCursorPos(ImVec2(20, 125));
    ImGui::Text("Arena: Sandbox");
    ImGui::SetCursorPos(ImVec2(20, 180));
    ImGui::Text("Practice your technique against custom enemies");
    ImGui::SetCursorPos(ImVec2(20, 235));
    ImGui::Text("Change the stats of the enemies to make it easier or harder");

    ImGui::SetCursorPos(ImVec2(20, 345));
    ImGui::PushItemWidth(200);
    ImGui::Combo("##ARENA_LOCATION", &arena_location, Constants::arena_locations, IM_ARRAYSIZE(Constants::arena_locations));
    ImGui::PopItemWidth();

    ImGui::PushItemWidth(200);
    ImGui::SetCursorPos(ImVec2(20, 375));

    if (ImGui::Button("Teleport to Arena")) {
        if (arena_location == 0) {
            CreateArenaVault();
            arenaCoordinates = Constants::vault_arena_coords;
            teleport(&arenaCoordinates);

        } else if (arena_location == 1) {
            CreateArenaUnderBridge();
            arenaCoordinates = Constants::bridge_arena_coords;
            teleport(&arenaCoordinates);

        } else {
            CreateArenaCowIsland();
            arenaCoordinates = Constants::cow_arena_coords;
            teleport(&arenaCoordinates);
        }
    }
    ImGui::PopItemWidth();

    ImGui::SetCursorPos(ImVec2(20, 430));
    ImGui::PushItemWidth(200);
    ImGui::Combo("##ENEMY_TO_SPAWN", &enemy_selected, Constants::enemies_to_select, IM_ARRAYSIZE(Constants::enemies_to_select));
    ImGui::PopItemWidth();

    ImGui::PushItemWidth(200);
    ImGui::SetCursorPos(ImVec2(20, 460));
    ImGui::Combo("##ENEMY_DAMAGE", &enemy_damage_selection, Constants::enemies_damage, IM_ARRAYSIZE(Constants::enemies_damage));
    ImGui::PopItemWidth();

    ImGui::PushItemWidth(200);
    ImGui::SetCursorPos(ImVec2(20, 490));
    ImGui::InputInt("##ENEMY_HEALTH", &enemy_health);
    ImGui::PopItemWidth();

    ImGui::PushItemWidth(200);
    ImGui::SetCursorPos(ImVec2(20, 520));
    ImGui::Combo("##ENEMY_NAME", &enemy_name, Constants::enemy_names, IM_ARRAYSIZE(Constants::enemy_names));
    ImGui::PopItemWidth();

    ImGui::SetCursorPos(ImVec2(20, 550));
    if (ImGui::Button("Spawn Enemy")) {
        Actor* enemy_spawned;

        switch (enemy_selected) {
        case 0:  // Giant Rat
            enemy_spawned = SpawnEnemy(GIANT_RAT_OFFSET, arenaCoordinates, enemy_name, enemy_health, enemy_damage_selection, spawn_vtable);
            break;
        case 1:  // Cave Spider
            enemy_spawned = SpawnEnemy(CAVE_SPIDER_OFFSET, arenaCoordinates, enemy_name, enemy_health, enemy_damage_selection, spawn_vtable);
            break;
        case 2:  // Ice Spider Queen
            enemy_spawned = SpawnEnemy(ICE_SPIDER_Q_OFFSET, arenaCoordinates, enemy_name, enemy_health, enemy_damage_selection, spawn_vtable);
            break;
        case 3:  // Bear
            enemy_spawned = SpawnEnemy(BEAR_OFFSET, arenaCoordinates, enemy_name, enemy_health, enemy_damage_selection, spawn_vtable);
            break;
        case 4:  // Angry Bear
            enemy_spawned = SpawnEnemy(ANGRY_BEAR_OFFSET, arenaCoordinates, enemy_name, enemy_health, enemy_damage_selection, spawn_vtable);
            break;
        case 5:  // Mad Cow
            enemy_spawned = SpawnEnemy(MAD_COW_OFFSET, arenaCoordinates, enemy_name, enemy_health, enemy_damage_selection, spawn_vtable);
            break;
        case 6:  // Cow King
            enemy_spawned = SpawnEnemy(COW_K_OFFSET, arenaCoordinates, enemy_name, enemy_health, enemy_damage_selection, spawn_vtable);
            break;
        case 7:  // Magmarok
            enemy_spawned = SpawnEnemy(MAGMAROK_OFFSET, arenaCoordinates, enemy_name, enemy_health, enemy_damage_selection, spawn_vtable);
            break;
        }
    }
}

void arena_campaign_tab() {
    /*
    Arena Campaign Tab within the hack menu
    */


    ImGui::SetCursorPos(ImVec2(20, 125));
    ImGui::Text("Arena: Campaign");

    ImGui::SetCursorPos(ImVec2(20, 180));
    ImGui::Text("Fight waves of enemies, collect buffs and items");

    ImGui::SetCursorPos(ImVec2(20, 235));
    ImGui::Text("Changing the difficulty changes the stats of the enemies");

    ImGui::SetCursorPos(ImVec2(120, 290));
    ImGui::PushItemWidth(200);
    ImGui::Combo("##CAMPAIGN_DIFFICULTY", &arena_campaign_difficulty, Constants::arena_campaign_difficulties, IM_ARRAYSIZE(Constants::arena_campaign_difficulties));
    ImGui::PopItemWidth();

    ImGui::SetCursorPos(ImVec2(120, 345));
    ImGui::PushItemWidth(200);
    ImGui::Combo("##ARENA_LOCATION", &arena_location, Constants::arena_locations, IM_ARRAYSIZE(Constants::arena_locations));
    ImGui::PopItemWidth();

    ImGui::SetCursorPos(ImVec2(150, 455));
    //for each arena player health/speed is increased and player equipped with different item
    if (ImGui::Button("Play Arena: Campaign")) {
        if (arena_location == 0) {
            //give player item and change their health/speed
            GiveItem(Constants::fireball);
            *(get_player_health_addy()) = 500;
            *(get_player_walking_speed_addy()) = 300;
            //create arena and teleport player to it
            CreateArenaVault();
            arenaCoordinates = Constants::vault_arena_coords;
            teleport(&arenaCoordinates);
            //reset variables back to initial values needed for campaign mode
            gameOver = false;
            setup = false;
            round_count = 1;
        }
        else if (arena_location == 1) {
            GiveItem(Constants::zero_cool);
            *(get_player_health_addy()) = 500;
            *(get_player_walking_speed_addy()) = 300;
            CreateArenaUnderBridge();
            arenaCoordinates = Constants::bridge_arena_coords;
            teleport(&arenaCoordinates);
            gameOver = false;
            setup = false;
            round_count = 1;

        }
        else {
            GiveItem(Constants::heap_spray);
            for (int i = 0; i < 50; i++) {
                GiveItem(Constants::shotgun_ammo);
            }
            *(get_player_health_addy()) = 500;
            *(get_player_walking_speed_addy()) = 300;
            CreateArenaCowIsland();
            arenaCoordinates = Constants::cow_arena_coords;
            teleport(&arenaCoordinates);
            gameOver = false;
            setup = false;
            round_count = 1;
        }
    }
}


////////////////////  IGNORE THIS STUFF BELOW //////////////////////////


typedef HRESULT(__stdcall* Present) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
typedef uintptr_t PTR;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Present oPresent;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;


LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;

    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

bool init = false;
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
    if (!init)
    {
        if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
        {
            pDevice->GetImmediateContext(&pContext);
            DXGI_SWAP_CHAIN_DESC sd;
            pSwapChain->GetDesc(&sd);
            window = sd.OutputWindow;
            ID3D11Texture2D* pBackBuffer;
            pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
            pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
            pBackBuffer->Release();
            oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();
            io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
            ImGui_ImplWin32_Init(window);
            ImGui_ImplDX11_Init(pDevice, pContext);
            init = true;
        }

        else
            return oPresent(pSwapChain, SyncInterval, Flags);
    }


    ////////////////////  IGNORE THIS STUFF ABOVE //////////////////////////


    if (show_window) {

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowSize(ImVec2(475, 650));

        if (ImGui::Begin("Team 6 Hack:  Please Give Us Lots Of Marks :)", &show_window)) {
            if (ImGui::BeginTabBar("TABS")) {
                if (ImGui::BeginTabItem("Player Settings")) {
                    player_settings_tab();
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Player Items")) {
                    player_items_tab();
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Arena: Sandbox")) {
                    arena_sandbox_tab();
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Arena: Campaign")) {
                    arena_campaign_tab();
                    ImGui::EndTabItem();
                }

            }
        }

        ImGui::End();

        ImGui::Render();

        pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }
    return oPresent(pSwapChain, SyncInterval, Flags);
}

DWORD WINAPI MyThread(HMODULE hModule)
{
    originalChatFunction = GameLogicDll + 0x551a0;

    //AllocConsole();
    //FILE* f = new FILE;
    //freopen_s(&f, "CONOUT$", "w", stdout);

    set_up_hook();
    overwrite_player_tick();

    /*
    Function hook the DirectX11 Window so we can inject our GUI :)
    */
    bool init_hook = false;
    do
    {
        if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
        {
            kiero::bind(8, (void**)&oPresent, hkPresent);
            init_hook = true;
        }
    } while (!init_hook);


    /*
    Show / Hide Our Hack Menu when the insert key key is pressed
    */


    while (true) {
        if (GetAsyncKeyState(0x2D) & 1) {  // when insert key pressed
            show_window = !show_window;  // toggle mod menu visibility
            Sleep(2);
        }
    }

    return 0;
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)MyThread,hModule, 0, nullptr));
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    } 
    return TRUE;
}

