#pragma once

#include <yaml.h>
#include <base/base.h>
#include <base/log.h>
#include <base/playerdb.h>
#include <Actor/Actor.h>
#include <Actor/Player.h>
#include <Packet/TextPacket.h>
#include <Math/Vec3.h>
#include <boost/algorithm/string/replace.hpp>

struct WorldBorder {
	int minX = -50000;
	int maxX = 50000;
	int minZ = -50000;
	int maxZ = 50000;

	template <typename IO> static inline bool io(IO f, WorldBorder &overworld, YAML::Node &node) {
		return f(overworld.minX, node["minX"]) &&
			   f(overworld.minZ, node["minZ"]) &&
			   f(overworld.maxX, node["maxX"]) &&
			   f(overworld.maxZ, node["maxZ"]);
	}
};

inline struct Settings {
	bool operatorsCanPassBorder = true;
	bool approachingBorderMessageEnabled = true;
	std::string approachingBorderMessage = "You will reach the %axis% axis of the world border in %number% blocks!";
	int informBefore = 10;
	bool reachedBorderMessageEnabled = true;
	std::string reachedBorderMessage = "You reached the world border!";
	std::string rejectTeleportMessage = "§cThe teleport was unsuccessful because the selected coordinates were outside the bounds of the world border";
	WorldBorder overworld;
	WorldBorder nether;
	WorldBorder end;

	template <typename IO> static inline bool io(IO f, Settings &settings, YAML::Node &node) {
		return f(settings.operatorsCanPassBorder, node["operatorsCanPassBorder"]) &&
			   f(settings.approachingBorderMessageEnabled, node["approachingBorderMessageEnabled"]) &&
			   f(settings.approachingBorderMessage, node["approachingBorderMessage"]) &&
			   f(settings.informBefore, node["informBefore"]) &&
			   f(settings.reachedBorderMessageEnabled, node["reachedBorderMessageEnabled"]) &&
			   f(settings.reachedBorderMessage, node["reachedBorderMessage"]) &&
			   f(settings.rejectTeleportMessage, node["rejectTeleportMessage"]) &&
			   f(settings.overworld, node["overworld"]) &&
			   f(settings.nether, node["nether"]) &&
			   f(settings.end, node["end"]);
	}
} settings;

DEF_LOGGER("WorldBorder");