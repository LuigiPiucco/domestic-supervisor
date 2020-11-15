#pragma once

#include "view.hpp"
#include <Wt/WAnchor.h>
#include <Wt/WSignal.h>

namespace widgets {
    class page_anchor : public Wt::WAnchor {
    public:
        page_anchor(view const &page);
    };
} // namespace widgets