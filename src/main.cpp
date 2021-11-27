#include "main.h"
#include <dllentry.h>

DEFAULT_SETTINGS(settings);

static std::unordered_map<int, WorldBorder> borders;

void dllenter() {}
void dllexit() {}
void PreInit() {
	borders[0] = settings.overworld;
	borders[1] = settings.nether;
	borders[2] = settings.end;
}
void PostInit() {}

inline void teleportInsideBorder(Player &player, const Vec3& destinationPos) {
	player.teleport(destinationPos, player.getPos(), player.getDimensionId().value);
	if (settings.reachedBorderMessageEnabled) {
		auto pkt = TextPacket::createTextPacket<TextPacketType::JukeboxPopup>(settings.reachedBorderMessage);
		player.sendNetworkPacket(pkt);
	}
}

void sendApproachMsg(Player &player, std::string axis, int number) {
	std::string out(settings.approachingBorderMessage);
	boost::replace_all(out, "%axis%", axis);
	boost::replace_all(out, "%number%", std::to_string(number));
	auto pkt = TextPacket::createTextPacket<TextPacketType::JukeboxPopup>(out);
	player.sendNetworkPacket(pkt);
}

THook(void, "?normalTick@Player@@UEAAXXZ", Player *player) {

	original(player);

	if (!player->isPlayerInitialized() || (settings.operatorsCanPassBorder &&
		player->getCommandPermissionLevel() > CommandPermissionLevel::Any)) return;

	auto currPos = player->getPos();
	currPos.y -= 1.62f;
	const auto& prevPos = player->getPosOld();

	WorldBorder border = borders[player->getDimensionId().value];
	bool hasMoved = (std::fabs(currPos.x - prevPos.x) >= 0.001f || std::fabs(currPos.z - prevPos.z) >= 0.001f);

	if (hasMoved) {

		std::string axis = "X";
		float min = std::fabs(border.maxX - currPos.x);

		if (std::fabs(border.minX - currPos.x) < min) {
			min = std::fabs(border.minX - currPos.x);
		}
		else if (std::fabs(border.maxZ - currPos.z) < min) {
			min = std::fabs(border.maxZ - currPos.z);
			axis = "Z";
		}
		else if (std::fabs(border.minZ - currPos.z) < min) {
			min = std::fabs(border.minZ - currPos.z);
			axis = "Z";
		}

		if (settings.approachingBorderMessageEnabled && (min > 0) && (min <= settings.informBefore)) {
			sendApproachMsg(*player, axis, (int)min);
		}

		if ((currPos.x <= border.minX) || (currPos.x >= border.maxX) || (currPos.z <= border.minZ) || (currPos.z >= border.maxZ)) {

			if (currPos.x <= border.minX) {
	  			currPos.x += std::fabs(currPos.x - border.minX) + 5.0f;
	  		}
			else if (currPos.x >= border.maxX) {
				currPos.x -= std::fabs(currPos.x - border.maxX) + 5.0f;
	  		}

	  		if (currPos.z <= border.minZ) {
	  			currPos.z += std::fabs(currPos.z - border.minZ) + 5.0f;
	  		}
	  		else if (currPos.z >= border.maxZ) {
	  			currPos.z -= std::fabs(currPos.z - border.maxZ) + 5.0f;
	  		}

	  		return teleportInsideBorder(*player, currPos);
		}
	}
}

THook(void, "?teleportTo@Player@@UEAAXAEBVVec3@@_NHHAEBUActorUniqueID@@@Z", Player &player, Vec3 const &pos,
	bool shouldStopRiding, int cause, int sourceEntityType, ActorUniqueID const &destinationEntityId) {

	if (settings.operatorsCanPassBorder && (player.getCommandPermissionLevel() > CommandPermissionLevel::Any)) {
		return original(player, pos, shouldStopRiding, cause, sourceEntityType, destinationEntityId);
	}

	WorldBorder border = borders[player.getDimensionId().value];

	if ((pos.x <= border.minX) || (pos.x >= border.maxX) || (pos.z <= border.minZ) || (pos.z >= border.maxZ)) {
		auto pkt = TextPacket::createTextPacket<TextPacketType::SystemMessage>(settings.rejectTeleportMessage);
		return player.sendNetworkPacket(pkt);
	}

	original(player, pos, shouldStopRiding, cause, sourceEntityType, destinationEntityId);
}