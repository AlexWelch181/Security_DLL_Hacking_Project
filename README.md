# Security Game Hack

## Disclaimer

This is a game hacking project to learn about the vulnerabilities of dll's

This was a project I worked on with a group of friends at University, I worked alone on the CodeInjectorConsole but collaborated with my 5 other friends on the security hack code. In the SecurityHack code I specifically worked on intercepting the calls to existing functions by hooking in our functions and then using assembly code to put the stack in the correct state. I also wrote a lot of spawning.cpp where I overwrote the vtables of objects in the game to influence their attributes.

## Injector

Note when using the code injector please make sure there are no spaces in your dll path

## To compile

1. Install the directx sdk https://www.microsoft.com/en-us/download/details.aspx?id=6812

2. Reboot computer

3. Open SecurityHack.sln in visual studio and compile as a x86 Release (See top of screen)

## Opening Hack Menu

Press the 'insert' key to toggle the cheat menu on / off

## Commands to execute simple hacks using the chat box

1. Change player stats:

- health x -> change health to value x
- mana x -> change mana to value x
- move x -> change movement speed to value x
- jump x -> change jump speed to value x
- time x -> change jump hold time to value x

2. Teleport the player:

- teleport x y z -> move player to position (x,y,z) on the map
- arena -> move player to cow island arena
- arena 1 -> move player to vault arena
- arena 2 -> move player to under bridge arena

3. Add items to inventory:

- fireball -> add fireball spell to inventory
- cube -> add rubix cube to inventory
- zerocool -> add ice spell to inventory
- akrifle -> add akrifle to inventory
- ropchaingun -> add rop chaingun to inventory
- goldenmaster -> add golden master to inventory
- heapspray -> add heap spray to inventory
- remotexploit -> add remote exploit to inventory
- cowboycoder -> add cowboy coder to inventory
- holyhandgrenade -> add holy hand grenade to inventory
- ammo -> add 50 of a range of ammunitions to inventory

4. Spawn enemy actors

- rat -> spawn a rat
- spider -> spawn a cave spider
- spider queen -> spawn a ice spider queen
- bear -> spawn a bear
- angry bear -> spawn an angry bear
- mad cow -> spawn a mad cow
- cow king -> spawn a cow king
- big boi -> spawn a magmarok

## How to play

Open the hack menu by pressing "insert" and then navigate to the arena campaign tab then select the mad cow island arena and press play campaign, the other arenas do not work due to AI Zone restrictions. We were working on creating our own custom AI Zones but due to time constraints could not finish this.
