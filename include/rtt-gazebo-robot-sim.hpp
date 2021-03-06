#ifndef COMAN_SIM_HPP
#define COMAN_SIM_HPP

#include <rtt/TaskContext.hpp>
#include <rtt/Port.hpp>
#include <rtt/Component.hpp>
#include <rtt/os/Semaphore.hpp>

#include <gazebo/gazebo.hh>
#include <gazebo/physics/physics.hh>
#include <gazebo/common/common.hh>

#include <Eigen/Dense>

#include <vector>

#include <Eigen/Core>
#include <time.h>
#include <rtt/os/TimeService.hpp>
#include <sstream>
#include <rtt/Logger.hpp>

#include <thread>
#include <memory>


#include <control_modes.h>
#include <kinematic_chain.h>
#include <force_torque_sensor.h>

#include <srdfdom_advr/model.h>
#include <urdf/model.h>
#include <parser.h>
#include <XBotCoreModel.h>

namespace cogimon {

class robotSim: public RTT::TaskContext {
public:
    robotSim(std::string const& name);
    bool configureHook();
    void updateHook();
    void WorldUpdateBegin();
    void WorldUpdateEnd();
    ~robotSim();

protected:
    bool getModel(const std::string& model_name);
    void gazeboUpdateHook(gazebo::physics::ModelPtr model);
    bool gazeboConfigureHook(gazebo::physics::ModelPtr model);
    bool setControlMode(const std::string& kinematic_chain, const std::string& controlMode);
    std::vector<std::string> getKinematiChains();
    std::string getControlMode(const std::string& kinematic_chain);
    std::vector<std::string> getControlAvailableMode(const std::string& kinematic_chain);
    std::string printKinematicChainInformation(const std::string& kinematic_chain);
    bool loadURDFAndSRDF(const std::string& URDF_path, const std::string& SRDF_path);
    std::map<std::string, std::vector<std::string> > getKinematiChainsAndJoints();
    bool resetModelConfiguration();
    bool setInitialPosition(const std::string& kin_chain, const std::vector<double>& init);
    std::vector<std::string> getForceTorqueSensorsFrames();

    gazebo::physics::ModelPtr model;
    gazebo::event::ConnectionPtr world_begin;
    gazebo::event::ConnectionPtr world_end;

    RTT::SendHandle<gazebo::physics::ModelPtr(const std::string&, double)> get_model_handle;

    gazebo::physics::Joint_V gazebo_joints_;
    gazebo::physics::Link_V model_links_;

    std::map<std::string, std::shared_ptr<KinematicChain>> kinematic_chains;
    std::vector<force_torque_sensor> force_torque_sensors;

    bool _models_loaded;
    XBot::XBotCoreModel _xbotcore_model;

    gain_parser gains;

private:
    bool is_configured;
};

}
#endif
