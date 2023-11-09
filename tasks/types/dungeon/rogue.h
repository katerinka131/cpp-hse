#pragma once

#include "dungeon.h"

#include <stdexcept>
#include <set>
#include <map>

Room* ans;
std::map<Room*, bool> us;
std::set<std::string> new_keys;
std::set<Door*> new_doors;

void Dfs(Room* room) {
    if (room->IsFinal()) {
        ans = room;
        return;
    }
    us[room] = true;
    size_t kol_keys = room->NumKeys();
    for (size_t i = 0; i < kol_keys; i++) {
        new_keys.insert(room->GetKey(i));
    }
    size_t kol_doors = room->NumDoors();
    for (size_t i = 0; i < kol_doors; i++) {
        new_doors.insert(room->GetDoor(i));
    }

    for (auto door : new_doors) {
        if (door->IsOpen()) {
            if (us.find(door->GoThrough()) == us.end()) {
                Dfs(door->GoThrough());
            }
        }
        for (auto key : new_keys) {
            if (door->TryOpen(key)) {
                if (us.find(door->GoThrough()) == us.end()) {
                    Dfs(door->GoThrough());
                }
                break;
            }
        }
    }
}

Room* FindFinalRoom(Room* starting_room) {
    new_keys = {};
    new_doors = {};
    us = {};
    ans = nullptr;
    Dfs(starting_room);
    return ans;
}
