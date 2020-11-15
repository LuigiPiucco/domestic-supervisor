#include "dynamic_image.hpp"

dynamic_image::~dynamic_image() { beingDeleted(); }

void dynamic_image::handleRequest(Wt::Http::Request const &request,
                                  Wt::Http::Response &response)
{
    Wt::WRasterImage res("png", ::width, ::height);

    for (std::size_t i = 0; i < ::height; ++i) {
        for (std::size_t j = 0; j < ::width; ++j) {
            auto pix = image(i, j);
            res.setPixel(j, i, {pix.red, pix.green, pix.blue});
        }
    }

    res.done();

    res.handleRequest(request, response);
}

void dynamic_image::set_image(const controllers::deepnet::image &im)
{
    image = im;

    setChanged();
}