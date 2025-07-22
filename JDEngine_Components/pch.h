#pragma once

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#include <vector>
#include <string>
#include <functional>
#include <unordered_map>
#include <algorithm>

#include <D2D1Helper.h> // d2d 구조체 및 Util 모음

#define _USE_MATH_DEFINES
#include <cmath>

#include <stdexcept>

