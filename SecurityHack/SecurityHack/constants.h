#pragma once

namespace Constants {
    static const char* cube = "RubicksCube";
    static const char* fireball = "GreatBallsOfFire";
    static const char* zero_cool = "ZeroCool";
    static const char* ak_rifle = "AKRifle";
    static const char* rop_chain_gun = "ROPChainGun";
    static const char* golden_master = "GoldenMaster";
    static const char* heap_spray = "HeapSpray";
    static const char* remote_exploit = "RemoteExploit";
    static const char* cowboy_coder = "CowboyCoder";
    static const char* holy_hand_grenade = "HolyHandGrenade";

    static const char* pistol_ammo = "PistolAmmo";
    static const char* rifle_ammo = "RifleAmmo";
    static const char* shotgun_ammo = "ShotgunAmmo";
    static const char* sniper_ammo = "SniperAmmo";
    static const char* revolver_ammo = "RevolverAmmo";

    static const char* weapons[7] = { "AkRifle", "RopChainGun", "GoldenMaster", "HeapSpray", "RemoteExploit", "CowboyCoder", "HolyHandGrenade" };
    
    static const char* weapon_selections[] = { "FireBall", "Zero Cool", "AK Rifle", "ROP Chain Gun", "Golden Master", "Heap Spray", "Remote Exploit", "Cowboy Coder", "Holy Hand Grenade", "Rubix Cube" };
    static const char* ammo_selections[] = { "Pistol Ammo", "Rifle Ammo", "Shotgun Ammo", "Sniper Ammo" };
    static const char* enemies_to_select[] = { "Giant Rat", "Cave Spider", "Ice Spider Queen", "Bear", "Angry Bear", "Mad Cow", "Cow King", "Magmarok" };
    static const char* enemies_damage[] = {"Easy: 50 attack dmg", "Medium: 250 attack damage", "Hard: 500 attack damage"};
    static const char* arena_locations[] = { "Arena: Vault", "Arena: Bridge", "Arena: Mad Cow Island" };
    static const char* arena_campaign_difficulties[] = { "Easy", "Normal", "Hard", "Actually Impossible" };
    static const char* enemy_names[] = { "Remy the rat", "Milka", "Halsin", "Angy Bear" };


    static Vec3 cow_arena_coords = { 256665.3281f, -249523.4688f, 1510.709f};
    static Vec3 vault_arena_coords = { -6861.130371f, 16136.91602f, 2270.150146f };
    static Vec3 bridge_arena_coords = { 18042.90234f, -13087.61035f, 99.0f };
}