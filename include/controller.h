#ifndef CONTROLLER_H_
#define CONTROLLER_H_
#include <map>

class RigCtrl;

class Controller {
    public:
        Controller();

    private:
        std::map<int, RigCtrl*> rigs;
        bool saveConfig();
};
#endif // CONTROLLER_H_