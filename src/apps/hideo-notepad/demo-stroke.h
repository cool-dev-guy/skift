#pragma once

#include <karm-ui/view.h>

#include "base.h"

namespace Demos {

static inline Demo STROKE_DEMO{
    Mdi::VECTOR_LINE,
    "Strokes",
    "Strokes styling and rendering",
    []() {
        return Ui::canvas(
            [](Gfx::Context &g, ...) {
                g.begin();
                g.translate({100, 100});
                g.moveTo({0, 0});
                g.lineTo({100, 100});
                g.lineTo({200, 0});

                g.strokeStyle(Gfx::stroke(Gfx::RED)
                                  .withCap(Gfx::StrokeCap::SQUARE_CAP)
                                  .withJoin(Gfx::StrokeJoin::MITER_JOIN)
                                  .withWidth(64));
                g.stroke();

                g.strokeStyle(Gfx::stroke(Gfx::BLUE)
                                  .withCap(Gfx::StrokeCap::ROUND_CAP)
                                  .withJoin(Gfx::StrokeJoin::ROUND_JOIN)
                                  .withWidth(64));
                g.stroke();

                g.strokeStyle(Gfx::stroke(Gfx::GREEN).withWidth(64));
                g.stroke();
            });
    },
};

} // namespace Demos
