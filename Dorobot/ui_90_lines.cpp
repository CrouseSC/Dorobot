#include "pch.h"
#include "ui_90_lines.h"
#include "Dorobot.h"

void UI_90Lines::render()
{
    Dorobot* doroBot = Dorobot::getInstance();
    if (!doroBot->uiMenu->lines_toggle) return;

    ImColor color(doroBot->uiMenu->lines_color);

    // Player position
    Vec3<float> player = doroBot->game->getOrigin();

    // Z = player's Z + slider offset (±100)
    float rel = doroBot->uiMenu->lines_rel_z;
    if (rel > 100.f) rel = 100.f;
    if (rel < -100.f) rel = -100.f;
    Vec3<float> origin(player.x, player.y, player.z + rel);

    // Draw 4 cardinal lines (world space)
    std::vector<std::pair<Game::GfxPointVertex, Game::GfxPointVertex>> verts;
    verts.reserve(4);
    for (int i = 0; i < 4; ++i)
    {
        const float length = 800.0f;
        Vec3<float> dir;
        mm::angle_vectors({ 0.0f, float(i * 90), 0.0f }, &dir, nullptr, nullptr);

        Vec3<float> end = origin + (dir * Vec3<float>(length, length, length));
        verts.push_back({
            Game::GfxPointVertex(origin, color),
            Game::GfxPointVertex(end,    color)
        });
    }

    doroBot->game->polyline(int(verts.size()), 1,
        reinterpret_cast<Game::GfxPointVertex*>(verts.data()), true);
}

UI_90Lines::UI_90Lines(Dorobot* /*d*/) {}
UI_90Lines::~UI_90Lines() {}