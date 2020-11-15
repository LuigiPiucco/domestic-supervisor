#pragma once

#include <Wt/WContainerWidget.h>
#include <string_view>

/// Defines that any `view`, aside from also being a `WCompositeWidget`, must
/// have a name and a path.
///
/// These two are implemented as pure virtual constexpr functions. Meaning they
/// belong to the class and are evaluated and substituted at compile-time.
class view : public Wt::WContainerWidget {
public:
    [[nodiscard]] constexpr virtual auto name() const -> std::string_view = 0;
    [[nodiscard]] constexpr virtual auto path() const -> std::string_view = 0;
};
