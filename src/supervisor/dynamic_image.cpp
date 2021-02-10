#include "dynamic_image.hpp"

dynamic_image::dynamic_image() : Wt::WRasterImage("jpeg", ::width, ::height)
{
    clear();
};

dynamic_image::~dynamic_image() { beingDeleted(); }

void dynamic_image::handleRequest(Wt::Http::Request const &request,
                                  Wt::Http::Response &response)
{
    response.addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    response.addHeader("Pragma", "no-cache");
    response.addHeader("Expires", "0");
    Wt::WRasterImage::handleRequest(request, response);
}

void dynamic_image::set_image(const controllers::deepnet::image &im)
{
    for (std::size_t i = 0; i < ::height; ++i) {
        for (std::size_t j = 0; j < ::width; ++j) {
            auto pix = im(i, j);
            setPixel(j, i, {pix.red, pix.green, pix.blue});
        }
    }
    done();

    Wt::WResource::setChanged();
}
