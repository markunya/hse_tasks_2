#pragma once

#include "dungeon.h"

#include <stdexcept>
#include <queue>
#include <unordered_set>

Room* FindFinalRoom(Room* starting_room) {
    if (starting_room->IsFinal()) {
        return starting_room;
    }
    size_t counter_of_opened = 0;
    std::queue<Door*> bfs_queue;
    std::unordered_set<Door*> used;
    std::unordered_set<std::string> keys;
    for (size_t i = 0; i < starting_room->NumDoors(); ++i) {
        if (starting_room->GetDoor(i)->IsOpen()) {
            ++counter_of_opened;
        }
        bfs_queue.emplace(starting_room->GetDoor(i));
        used.insert(starting_room->GetDoor(i));
    }
    for (size_t i = 0; i < starting_room->NumKeys(); ++i) {
        keys.insert(starting_room->GetKey(i));
    }
    while (!bfs_queue.empty()) {
        if (counter_of_opened == 0) {
            size_t len = bfs_queue.size();
            while (len > 0) {
                Door* current = bfs_queue.front();
                bool flag = false;
                for (auto key : keys) {
                    if (current->TryOpen(key)) {
                        flag = true;
                        break;
                    }
                }
                if (flag) {
                    break;
                }
                bfs_queue.pop();
                bfs_queue.emplace(current);
                --len;
            }
            if (len == 0) {
                return nullptr;
            }
        }
        Door* current = bfs_queue.front();
        bfs_queue.pop();
        if (!current->IsOpen()) {
            for (auto key : keys) {
                if (current->TryOpen(key)) {
                    break;
                }
            }
        }
        if (!current->IsOpen()) {
            bfs_queue.emplace(current);
            continue;
        }
        --counter_of_opened;
        Room* room = current->GoThrough();
        if (room->IsFinal()) {
            return room;
        }
        for (size_t i = 0; i < room->NumDoors(); ++i) {
            if (!used.count(room->GetDoor(i))) {
                if (room->GetDoor(i)->IsOpen()) {
                    ++counter_of_opened;
                }
                bfs_queue.emplace(room->GetDoor(i));
                used.insert(room->GetDoor(i));
            }
        }
        for (size_t i = 0; i < room->NumKeys(); ++i) {
            keys.insert(room->GetKey(i));
        }
    }
    return nullptr;
}
