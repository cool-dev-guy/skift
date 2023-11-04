#pragma once

#include <karm-ui/view.h>

#include "base.h"

namespace Demos {

static inline Demo CIRCLE_DEMO{
    Mdi::CIRCLE,
    "Circles",
    "Circles rendering",
    []() {
        return Ui::canvas(
            [](Gfx::Context &g, Math::Vec2i size) {
                Math::Rand rand{};

                for (isize i = 0; i < 10; i++) {
                    f64 s = rand.nextInt(4, 10);
                    s *= s;

                    g.begin();
                    g.ellipse({
                        rand.nextVec2(size).cast<f64>(),
                        s,
                    });

                    g.strokeStyle(
                        Gfx::stroke(Gfx::randomColor(rand))
                            .withWidth(rand.nextInt(2, s)));
                    g.stroke();
                }
            });
    },
};

} // namespace Demos
