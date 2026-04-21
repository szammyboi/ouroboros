#include "ui/bodyInfo.h"
#include "global.h"

constexpr float PI = 3.14159265358979323846f;

void AddOrbitingBody(float r, float mass)
{
    Body b;

    Body& center = Global::GetSim().Bodies[Global::GetSettings().selectedBody];

    glm::vec3 centerPos = center.loc;
    glm::vec3 centerVel = center.vel;
    float M = center.mass;

    glm::vec3 dir = glm::vec3(1, 0, 0);
    b.loc = centerPos + dir * r;

    glm::vec3 up = glm::vec3(0, 1, 0);
    glm::vec3 tangent = glm::normalize(glm::cross(dir, up));

    float v = sqrt(M / r);
    b.vel = tangent * v;

    b.vel += centerVel;

    b.mass = mass;
    b.radius = 1e-5f;

    Global::GetSim().add_body(b);
}

void BodyInfo::OnUpdate(){
    int idx = Global::GetSettings().selectedBody;
    if (idx == -1) {
        placing = false;
        return;
    }

    Sim& sim = Global::GetSim();
    if (idx >= int(sim.Bodies.size())) return;

    Body& body = sim.Bodies[idx];

    ImGui::SetNextWindowSize(ImVec2(260, 0), ImGuiCond_Always);
    ImGui::Begin("Body Info", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);

    ImGui::Text("Body #%d", idx);
    ImGui::Separator();
    ImGui::Text("Mass:     %.10e (Solar Mass)", body.mass);
    ImGui::Text("Position: %.2f  %.2f  %.2f", body.loc.x, body.loc.y, body.loc.z);
    ImGui::Text("Velocity: %.2f  %.2f  %.2f", body.vel.x, body.vel.y, body.vel.z);
    ImGui::Text("Accel:    %.2f  %.2f  %.2f", body.acc.x, body.acc.y, body.acc.z);
    ImGui::SliderFloat("Temp: ", &body.temperature, 1500, 20'000);;

    static float orbitRadius = 1.0f;   // AU
    static float bodyMass    = 1e-6f;  // solar masses

    if (ImGui::Button("Delete Body")) {
        body.mass = 1e-15f;
        Global::GetSettings().selectedBody = -1;
    }
    if (ImGui::Button("Add Orbiting Body")) {
        placing = true;
    }

    if (placing) {
        ImGui::SliderFloat("Orbit Dist (AU)", &orbitRadius, 0.01f, 100.0f, "%.3f", ImGuiSliderFlags_Logarithmic);
        ImGui::InputFloat("Body Mass (solar masses)", &bodyMass, 0, 0, "%.6e");

        if (ImGui::Button("Finalize")) {
            AddOrbitingBody(orbitRadius, bodyMass);
        }
    }

    ImGui::End();
}