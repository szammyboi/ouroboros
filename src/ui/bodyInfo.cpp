#include "ui/bodyInfo.h"
#include "global.h"

void BodyInfo::OnUpdate(){
    int idx = Global::GetSettings().selectedBody;
    if (idx == -1) return;

    Sim& sim = Global::GetSim();
    if (idx >= int(sim.Bodies.size())) return;

    Body& body = sim.Bodies[idx];

    ImGui::SetNextWindowSize(ImVec2(260, 0), ImGuiCond_Always);
    ImGui::Begin("Body Info", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);

    ImGui::Text("Body #%d", idx);
    ImGui::Separator();
    ImGui::Text("Mass:     %.3e", body.mass);
    ImGui::Text("Position: %.2f  %.2f  %.2f", body.loc.x, body.loc.y, body.loc.z);
    ImGui::Text("Velocity: %.2f  %.2f  %.2f", body.vel.x, body.vel.y, body.vel.z);
    ImGui::Text("Accel:    %.2f  %.2f  %.2f", body.acc.x, body.acc.y, body.acc.z);

    ImGui::End();
}