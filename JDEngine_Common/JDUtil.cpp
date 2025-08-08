#include "pch.h"
#include "framework.h"
#include "JDUtil.h"

using Resource = JDGlobal::Contents::Resource;
using CatType = JDGlobal::Contents::CatType;

JDGlobal::Contents::Resource JDUtil::GetCatBonus(JDGlobal::Contents::CatType cat)
{
    switch (cat) {
    case CatType::Felis: return Resource{ 10,0,0 };
    case CatType::Navi: return Resource{ 0,10,0 };
    case CatType::Kone: return Resource{ 0,0,10 };
    default: return Resource{ 0,0,0 };
    }
}
