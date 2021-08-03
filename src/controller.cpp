#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <httplib.h>
#include "../include/controller.h"
#include "../include/rigctrl.h"
#include "../include/util.h"

#define JSON_CONTENT_TYPE "application/json"

bool Controller::saveConfig() {
    nlohmann::json data;
    for (auto const& x : this->rigs) {
        auto c = x.second->getConfig();
        data.push_back(nlohmann::json::object({
            {"id", c.id},
            {"model", c.model},
            {"serial_rate", c.serial_rate},
            {"data_bits",c.data_bits},
            {"stop_bits", c.stop_bits},
            {"handshake", c.handshake},
            {"port_path", c.port_path},
            {"started", c.started}
        }));
    }
    Util::saveFile("rig.json", data.dump());
    return true;
}
Controller::Controller() {
    std::cout << "Controller Initialized" << std::endl;
    auto cfg = nlohmann::json::parse(Util::readFile("rig.json", "[]"));
    for (auto& e: cfg) {
        std::cout << "Config: " << e << std::endl;
        auto r = new RigCtrl();
        r->setConfig({
            e["id"].get<int>(),
            e["model"].get<int>(),
            e["serial_rate"].get<int>(),
            e["data_bits"].get<int>(),
            e["stop_bits"].get<int>(),
            strcpy(new char[e["handshake"].get<std::string>().length() + 1], e["handshake"].get<std::string>().c_str()),
            strcpy(new char[e["port_path"].get<std::string>().length() + 1], e["port_path"].get<std::string>().c_str()),
            e["started"].get<bool>()

        });
        if (e["started"].get<bool>()) {
            r->start();
        }
        this->rigs.insert(std::pair<int, RigCtrl*>(e["id"].get<int>(), r));
    }

    httplib::Server svr;

    svr.Get("/", [](const httplib::Request & req, httplib::Response &res) {
        res.set_content("list rigs", "text/plain");
    });
    
    svr.Get("/add", [&](const httplib::Request & req, httplib::Response &res) {
        
        auto r = new RigCtrl();
        r->setConfig({
            (int) this->rigs.size()+1,
            132,
            32800,
            8,
            1,
            (char*)"hardware",
            (char*)"/dev/ttyUSB0",
            false
        });
        this->rigs.insert(std::pair<int, RigCtrl*>(this->rigs.size()+1, r));
        std::cout << "Num Rigs: " << this->rigs.size() << std::endl;
        res.set_content("add rig", "text/plain");
        this->saveConfig();
    });

    svr.Get("/tty", [&](const httplib::Request & req, httplib::Response &res) {
        nlohmann::json response;
        auto ports = Util::listTTY();
        for (std::string i: ports) {
            response.push_back(i);
        }
        res.set_content(response.dump(), JSON_CONTENT_TYPE);
    });

    svr.Get("/receivers", [&](const httplib::Request & req, httplib::Response &res) {
        nlohmann::json response;
        auto rigs = RigCtrl::listRigs();
        for (RigType i: rigs) {
            response.push_back(nlohmann::json::object({
            {"id", i.id},
            {"manufacture", i.manufacture},
            {"name", i.name}
            }));
        }
        res.set_content(response.dump(), JSON_CONTENT_TYPE);
    });

    svr.Get(R"(/(\d+))", [&](const httplib::Request & req, httplib::Response &res) {
        auto rig_number = stoi(req.matches[1]);
        if (this->rigs.find(rig_number) == this->rigs.end()) {
            std::cout << "Rig not found" << std::endl;
            res.set_content("Rig not found", "text/plain");
        } else {
            std::cout << "Rig found: " << rig_number << std::endl;
            auto cfg = this->rigs.at(rig_number)->getConfig();
            nlohmann::json response;
            response["id"] = cfg.id;
            response["model"] = cfg.model;
            response["serial_rate"] = cfg.serial_rate;
            response["data_bits"] = cfg.data_bits;
            response["stop_bits"] = cfg.stop_bits;
            response["handshake"] = cfg.handshake;
            response["port_path"] = cfg.port_path;
            response["started"] = cfg.started;
            std::cout << "response" << response << std::endl;
            res.set_content(response.dump(), JSON_CONTENT_TYPE);
        }
        
    });

    svr.Post(R"(/(\d+))", [&](const httplib::Request &req, httplib::Response &res, const httplib::ContentReader &content_reader) {
        auto rig_number = stoi(req.matches[1]);
        if (this->rigs.find(rig_number) == this->rigs.end()) {
            std::cout << "Rig not found" << std::endl;
            res.set_content("Rig not found", "text/plain");
        } else {
      std::string body;
      content_reader([&](const char *data, size_t data_length) {
        body.append(data, data_length);
        return true;
      });
      auto data = nlohmann::json::parse(body);
      std::cout << "data: " << data << std::endl;
      this->rigs.at(rig_number)->setConfig({
            data["id"].get<int>(),
            data["model"].get<int>(),
            data["serial_rate"].get<int>(),
            data["data_bits"].get<int>(),
            data["stop_bits"].get<int>(),
            strcpy(new char[data["handshake"].get<std::string>().length() + 1], data["handshake"].get<std::string>().c_str()),
            strcpy(new char[data["port_path"].get<std::string>().length() + 1], data["port_path"].get<std::string>().c_str()),
            data["started"].get<bool>()
        });
        this->saveConfig();
        if (data["started"].get<bool>()) {
           this->rigs.at(rig_number)->start(); 
        }
      res.set_content(data.dump(), JSON_CONTENT_TYPE);

        }
        
    });
    svr.Get(R"(/(\d+)/tune)", [&](const httplib::Request & req, httplib::Response &res) {
        auto rig_number = stoi(req.matches[1]);
        if (this->rigs.find(rig_number) == this->rigs.end()) {
            std::cout << "Rig not found" << std::endl;
            res.set_content("Rig not found", "text/plain");
        } else {
            std::cout << "Rig found: " << rig_number << std::endl;
            auto cfg = this->rigs.at(rig_number)->getFreq();

            nlohmann::json response;
            response["frequency"] = cfg;
            std::cout << "response" << response << std::endl;
            res.set_content(response.dump(), JSON_CONTENT_TYPE);
        }
        
    });
svr.listen("0.0.0.0", 8080);
}