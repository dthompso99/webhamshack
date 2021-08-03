#ifndef RIGCTRL_H_
#define RIGCTRL_H_
#include <string>
#include <hamlib/rig.h>

struct RigConfig {
            int id;
            int model;
            int serial_rate;
            int data_bits;
            int stop_bits;
            char* handshake;
            char* port_path;
            bool started;
};


struct RigType {
    int id;
    std::string manufacture;
    std::string name;
};


class RigCtrl {
    public:
        RigCtrl();
        bool setConfig(RigConfig config);
        RigConfig getConfig();
        static std::vector<RigType> listRigs();
        static int rigcallback(const struct rig_caps *rc, rig_ptr_t);
        bool start();
        bool stop();
        double getFreq();
    private:
        RIG *thisrig;
        RigConfig _cfg;
        bool _started;
        
};
#endif // RIGCTRL_H_