#pragma once
#include <include/json.hpp>
#include "vectors.h"
#include "cod4_structs.h"
#include "recording.h"
#include "bind_manager.h"

inline void to_json(nlohmann::json& j, const Vec3<float>& v)
{
    j = nlohmann::json{ {"x", v.x}, {"y", v.y}, {"z", v.z} };
}

inline void to_json(nlohmann::json& j, const usercmd_s& c)
{
	j = nlohmann::json{ {"serverTime", c.serverTime}, {"buttons", c.buttons}, {"angles", c.angles}, {"weapon", c.weapon}, {"offHandIndex", c.offHandIndex},
						{"forward", c.forward}, {"side", c.side}, {"meleeChargeYaw", c.meleeChargeYaw}, {"meleeChargeDist", c.meleeChargeDist},
						{"selectedLocation", c.selectedLocation}};
}

inline void to_json(nlohmann::json& j, const CommandPacket& p)
{
    j = nlohmann::json{ {"c", p.cmd}, {"o", p.origin}, {"v", p.velocity}, {"nv", p.normalView} };
}

inline void to_json(nlohmann::json& j, const Bind& b)
{
    j = nlohmann::json{ {"id", b.id}, {"key", b.key}, {"nm", b.name}, {"type", b.type} };
}

inline void from_json(const nlohmann::json& j, Bind& b)
{
    j.at("id").get_to(b.id);
	j.at("key").get_to(b.key);
    j.at("nm").get_to(b.name);
	j.at("type").get_to(b.type);
	b.active = false;
}

inline void from_json(const nlohmann::json& j, Vec3<float>& v)
{
    j.at("x").get_to(v.x);
    j.at("y").get_to(v.y);
    j.at("z").get_to(v.z);
}

inline void from_json(const nlohmann::json& j, usercmd_s& c)
{
    j.at("serverTime").get_to(c.serverTime);
    j.at("buttons").get_to(c.buttons);
    j.at("angles").get_to(c.angles);
    j.at("weapon").get_to(c.weapon);
    j.at("offHandIndex").get_to(c.offHandIndex);
    j.at("forward").get_to(c.forward);
    j.at("side").get_to(c.side);
    j.at("meleeChargeYaw").get_to(c.meleeChargeYaw);
    j.at("meleeChargeDist").get_to(c.meleeChargeDist);
    j.at("selectedLocation").get_to(c.selectedLocation);
}

inline void from_json(const nlohmann::json& j, CommandPacket& p)
{
    j.at("c").get_to(p.cmd);
    j.at("o").get_to(p.origin);
    j.at("v").get_to(p.velocity);
    j.at("nv").get_to(p.normalView);
}
