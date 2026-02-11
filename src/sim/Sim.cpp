#include "Sim.h"
#include "spdlog/spdlog.h"

namespace Ouroboros{
    Sim::Sim(){
        spdlog::info("Sim Created...");
    }

    void Sim::step(float dt){
        // spdlog::info("Stepping {}...", dt);
    }
}