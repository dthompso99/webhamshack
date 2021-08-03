#include <iostream>
#include <vector>
#include <cstring>
#include "../include/rigctrl.h"


RigCtrl::RigCtrl() {

}

double RigCtrl::getFreq() {
    freq_t freq;
    if (this->_started) {
        auto e = rig_get_freq(this->thisrig, RIG_VFO_CURR, &freq);
        if (e == 0) {
            return static_cast<double>(freq);
        } else {
            return {0.00};
        }   
    } else {
        return {0.00};
    }
}

bool RigCtrl::start() {
    rig_model_t myrig_model;
    if (this->_started == true) {
        std::cout << "ERROR: Allready Started" << std::endl;
        return 0;
    } else if (this->_cfg.started == true) {
        hamlib_port_t myport;
        myport.type.rig = RIG_PORT_SERIAL;
        myport.parm.serial.rate = this->_cfg.serial_rate;
        myport.parm.serial.data_bits = this->_cfg.data_bits;
        myport.parm.serial.stop_bits = this->_cfg.stop_bits;
        myport.parm.serial.parity = RIG_PARITY_NONE;
        if (this->_cfg.handshake == "hardware"){
                myport.parm.serial.handshake = RIG_HANDSHAKE_HARDWARE;
        } else {
                myport.parm.serial.handshake = RIG_HANDSHAKE_NONE;
        }
        std::strncpy(myport.pathname, this->_cfg.port_path, FILPATHLEN - 1);

        rig_load_all_backends();
        myrig_model = this->_cfg.model;
        this->thisrig = rig_init(myrig_model);
        strncpy(this->thisrig->state.rigport.pathname, this->_cfg.port_path, FILPATHLEN - 1);
        //myrig_model = rig_probe(&myport);
        auto retcode = rig_open(this->thisrig);
        std::cout << "rig start: " << retcode << std::endl;
        this->_started = true;
        return 1;
    }
}

bool RigCtrl:: stop() {
    if (!this->_cfg.started) {
        std::cout << "ERROR: Allready Stopped" << std::endl;
        return 0;
    } else {
        return 1;
    }
}

bool RigCtrl::setConfig(RigConfig cfg) {
    this->_cfg = cfg;
    return true;
}

RigConfig RigCtrl::getConfig() {
    return this->_cfg;
}

rig_ptr_t list_rigs_ptr;
std::vector<RigType> rigtypes;

std::vector<RigType> RigCtrl::listRigs() {
    if (rigtypes.empty()) {
        rig_load_all_backends();
        rig_list_foreach(RigCtrl::rigcallback, list_rigs_ptr);
    }
    return rigtypes;
}

int RigCtrl::rigcallback(const struct rig_caps *rc, rig_ptr_t) {
            rigtypes.push_back({
                rc->rig_model,
                rc->mfg_name,
                rc->model_name
            });
        return 1;
}