#include "page_anchor.hpp"
#include <Wt/WLink.h>
#include <string>

widgets::page_anchor::page_anchor(view const &page)
    : Wt::WAnchor(
          Wt::WLink(Wt::LinkType::InternalPath, std::string(page.path())),
          std::string(page.name()))
{
    setContentAlignment(Wt::AlignmentFlag::Center);
    setVerticalAlignment(Wt::AlignmentFlag::Middle);

    constexpr auto width = 290;
    constexpr auto height = 64;
    resize(width, height);
    addCssRule("#" + id(),
               "background: rgba(139, 233, 253, 0.25); backdrop-filter: "
               "blur(32px); border-radius: 32px; color: rgba(189, 147, 249, "
               "0.75); text-decoration-line: underline; text-shadow: 0px 4px "
               "4px rgba(0, 0, 0, 0.25);");
    addCssRule("#" + id() + ">span", "font-size: 36px; font-weight: bold; "
                                     "line-height: 64px; height: 64;");
}