#pragma once

#include "../Server.h"
#include <vector>
#include <string>
#include <ctime>
#include <deque>
#include <../../json/json.h>
#include "../datamanage.h"
#include "../tool.h"
using namespace std;
#include <filesystem>
#include "chatroom.h"

vector<chatroom> room;

bool used[MAXROOM];
int addroom();

void delroom(int x);