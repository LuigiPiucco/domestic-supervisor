#pragma once

#include "domestic_supervisor.hpp"
#include <Wt/WEnvironment.h>
#include <Wt/WServer.h>
#include <deepnet.hpp>
#include <mqtt.hpp>
#include <string>

class domestic_server : public Wt::WServer {
public:
    domestic_server(int argc, char **argv);

private:
    controllers::mqtt mqtt;
    controllers::deepnet deepnet;

    std::shared_ptr<Wt::WRasterImage> bg;
};