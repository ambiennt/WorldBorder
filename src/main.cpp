#include "global.h"
#include <dllentry.h>

Settings settings;
DEFAULT_SETTINGS(settings);

void dllenter() {}
void dllexit() {}

std::unordered_map<int, WorldBorder> borders;

void PreInit() {
  borders[0] = settings.overworld;
  borders[1] = settings.nether;
  borders[2] = settings.end;
}
void PostInit() {}

void teleport(Player &thi, float x, float y, float z) {
  Vec3 teleportTo;
  teleportTo.x = x;
  teleportTo.y = y - 1.62;//camera is 1.62 blocks off ground
  teleportTo.z = z;
  Vec3 oldPos = thi.getPos();
  oldPos.y = oldPos.y - 1.62;
  thi.teleport(teleportTo, oldPos, thi.getDimensionId());
  auto pk = TextPacket::createTextPacket<TextPacketType::JukeboxPopup>(settings.hasReachedBorderMessage);
  if (settings.hasReachedBorderMessageEnabled) {
      thi.sendNetworkPacket(pk);
  }
}

void approachMsg(Player &thi, std::string pos, int number) {
  std::string out(settings.approachingBorderMessage);
  boost::replace_all(out, "%coord%", pos);
  boost::replace_all(out, "%number%", std::to_string(number));
  auto pk = TextPacket::createTextPacket<TextPacketType::JukeboxPopup>(out);
  thi.sendNetworkPacket(pk);
}

std::unordered_map<Player *, Vec3> prevPosition;

THook(void *, "?tickWorld@Player@@UEAAHAEBUTick@@@Z", Player *thi, struct Tick const &a) {
    if (settings.operatorsCanPassBorder) {
        if (thi->getCommandPermissionLevel() > CommandPermissionLevel::Any) return original(thi, a);
    }
    void* ret = original(thi, a); 
    Vec3 prevPos = prevPosition[thi];
    Vec3 currPos = thi->getPos();
    if (currPos == prevPos) return ret;
    if (std::fabs(currPos.x - prevPos.x) >= 0.00001 || std::fabs(currPos.z - prevPos.z) >= 0.00001) {
      WorldBorder border = borders[thi->getDimensionId().value];
      std::string coord = "X";
      float min = std::fabs(border.maxX - currPos.x);
      if (std::fabs(border.minX - currPos.x) < min) { min = std::fabs(border.minX - currPos.x); }
      if (std::fabs(border.maxZ - currPos.z) < min) {
        min = std::fabs(border.maxZ - currPos.z);
        coord = "Z";
      }
      if (std::fabs(border.minZ - currPos.z) < min) {
        min = std::fabs(border.minZ - currPos.z);
        coord = "Z";
      }
      if (((int) min) > 0 && ((int) min) <= settings.informBefore) approachMsg(*thi, coord, min);

      if (currPos.x >= border.maxX) {
          teleport(*thi, prevPos.x - 5, prevPos.y, prevPos.z);
      }
      else if (currPos.x <= border.minX) {
          teleport(*thi, prevPos.x + 5, prevPos.y, prevPos.z);
      }
      else if (currPos.z >= border.maxZ) {
          teleport(*thi, prevPos.x, prevPos.y, prevPos.z - 5);
      }
      else if (currPos.z <= border.minZ) {
          teleport(*thi, prevPos.x, prevPos.y, prevPos.z + 5);
      }
    }
    prevPosition[thi] = thi->getPos();
    return ret;
}

THook(
    void *, "?teleportTo@Player@@UEAAXAEBVVec3@@_NHHAEBUActorUniqueID@@@Z", Player &thi, Vec3 const &pos, bool b1,
    int i1, int i2, const ActorUniqueID *acuid) {
  if (settings.operatorsCanPassBorder) {
      if (thi.getCommandPermissionLevel() > CommandPermissionLevel::Any) return original(thi, pos, b1, i1, i2, acuid);
  }
  Vec3 prevPos = thi.getPos();
  WorldBorder border = borders[thi.getDimensionId().value];
  if (pos.x >= border.maxX || pos.x <= border.minX || pos.z >= border.maxZ || pos.z <= border.minZ) {
    teleport(thi, prevPos.x, prevPos.y, prevPos.z);
    return nullptr;
  }
  return original(thi, pos, b1, i1, i2, acuid);
}