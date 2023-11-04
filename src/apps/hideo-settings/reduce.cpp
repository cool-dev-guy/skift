#include <karm-main/base.h>

#include "app.h"

namespace Settings {

void reduce(State &s, Action action) {
    action.visit(Visitor{
        [&](GoTo a) {
            if (s.page() == a.page)
                return;

            s.history.removeRange(s.historyIndex + 1, s.history.len() - s.historyIndex - 1);
            s.history.pushBack(a.page);
            s.historyIndex = s.history.len() - 1;
        },
        [&](GoBack) {
            s.historyIndex = max(s.historyIndex - 1, 0uz);
        },
        [&](GoForward) {
            s.historyIndex = min(s.historyIndex + 1, s.history.len() - 1);
        },
    });
}

} // namespace Settings
