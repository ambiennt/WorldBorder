#pragma once

#include <yaml.h>
#include <string>
#include <map>
#include <base/log.h>
#include "base/playerdb.h"
#include <Actor/Actor.h>
#include <Actor/Player.h>
#include <Packet/TextPacket.h>
#include <boost/algorithm/string/replace.hpp>
#include <exception>
#include <Block/BlockSource.h>
#include <Block/Block.h>

struct WorldBorder {
  long long minX = -30000;
  long long maxX = 30000;
  long long minZ = -30000;
  long long maxZ = 30000;

  template <typename IO> static inline bool io(IO f, WorldBorder &overworld, YAML::Node &node) {
    return f(overworld.minX, node["minX"]) &&
           f(overworld.minZ, node["minZ"]) &&
           f(overworld.maxX, node["maxX"]) &&
           f(overworld.maxZ, node["maxZ"]);
  }
};

struct Settings {
  bool operatorsCanPassBorder = true;
  std::string approachingBorderMessage = "You will reach the %coord% axis of the world border in %number% blocks!";
  int informBefore = 10;
  bool hasReachedBorderMessageEnabled = true;
  std::string hasReachedBorderMessage = "You reached the world border!";
  WorldBorder overworld;
  WorldBorder nether;
  WorldBorder end;

  template <typename IO> static inline bool io(IO f, Settings &settings, YAML::Node &node) {
    return f(settings.operatorsCanPassBorder, node["operatorsCanPassBorder"]) &&
           f(settings.approachingBorderMessage, node["approachingBorderMessage"]) &&
           f(settings.informBefore, node["informBefore"]) &&
           f(settings.hasReachedBorderMessageEnabled, node["hasReachedBorderMessageEnabled"]) &&
           f(settings.hasReachedBorderMessage, node["hasReachedBorderMessage"]) &&
           f(settings.overworld, node["overworld"]) &&
           f(settings.nether, node["nether"]) &&
           f(settings.end, node["end"]);
  }
};

DEF_LOGGER("WorldBorder");

extern Settings settings;