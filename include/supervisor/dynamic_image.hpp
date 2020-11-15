#pragma once

#include "deepnet.hpp"
#include <Wt/Http/Request.h>
#include <Wt/Http/Response.h>
#include <Wt/WApplication.h>
#include <Wt/WBrush.h>
#include <Wt/WColor.h>
#include <Wt/WPainter.h>
#include <Wt/WRasterImage.h>
#include <Wt/WResource.h>

class dynamic_image : public Wt::WResource {
public:
    dynamic_image() = default;
    dynamic_image(dynamic_image const &d) = delete;
    dynamic_image(dynamic_image &&d) = delete;
    auto operator=(dynamic_image const &d) -> dynamic_image & = delete;
    auto operator=(dynamic_image &&d) -> dynamic_image & = delete;
    ~dynamic_image() override;

    void set_image(controllers::deepnet::image const &im);

    void handleRequest(Wt::Http::Request const &request,
                       Wt::Http::Response &response) override;

private:
    controllers::deepnet::image image;
};