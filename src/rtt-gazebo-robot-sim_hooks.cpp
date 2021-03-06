#include <rtt-gazebo-robot-sim.hpp>
#include <Eigen/Dense>

using namespace cogimon;
using namespace RTT;
using namespace RTT::os;
using namespace Eigen;

void robotSim::WorldUpdateBegin() {
    if (!is_configured && !isRunning())
        return;

    std::map<std::string, std::shared_ptr<KinematicChain>>::iterator it;
    for(it = kinematic_chains.begin(); it != kinematic_chains.end(); it++)
        it->second->getCommand();

    for(it = kinematic_chains.begin(); it != kinematic_chains.end(); it++)
        it->second->sense();

    for(unsigned int i = 0; i < force_torque_sensors.size(); ++i)
        force_torque_sensors[i].sense();
}

void robotSim::WorldUpdateEnd() {
    if (!is_configured && !isRunning())
        return;

    std::map<std::string, std::shared_ptr<KinematicChain>>::iterator it;
    for(it = kinematic_chains.begin(); it != kinematic_chains.end(); it++)
        it->second->move();
}
