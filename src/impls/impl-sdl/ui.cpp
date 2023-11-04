#include <SDL.h>
#include <karm-ui/drag.h>

#include <karm-ui/_embed.h>

namespace Karm::Ui::_Embed {

struct SdlHost :
    public Ui::Host {
    SDL_Window *_window{};

    Math::Vec2i _lastMousePos{};
    Math::Vec2i _lastScreenMousePos{};

    SdlHost(Ui::Child root, SDL_Window *window)
        : Ui::Host(root), _window(window) {
    }

    ~SdlHost() {
        SDL_DestroyWindow(_window);
        SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
        SDL_Quit();
    }

    Gfx::MutPixels mutPixels() override {
        SDL_Surface *s = SDL_GetWindowSurface(_window);

        return {
            s->pixels,
            {s->w, s->h},
            (usize)s->pitch,
            Gfx::BGRA8888,
        };
    }

    void flip(Slice<Math::Recti>) override {
        SDL_UpdateWindowSurface(_window);
    }
    // cool-guy-dev addition begin->
    Karm::Rune mapSDLKeyToKey(SDL_Keycode sdlKey) {
        switch (sdlKey) {
            case 0: return 0x00;
            case SDLK_ESCAPE: return 0x1B;
            case SDLK_1: return '1';
            case SDLK_2: return '2';
            case SDLK_3: return '3';
            case SDLK_4: return '4';
            case SDLK_5: return '5';
            case SDLK_6: return '6';
            case SDLK_7: return '7';
            case SDLK_8: return '8';
            case SDLK_9: return '9';
            case SDLK_0: return '0';
            case SDLK_MINUS: return '-';
            case SDLK_EQUALS: return '=';
            case SDLK_BACKSPACE: return '\b';
            case SDLK_TAB: return '\t';
            case SDLK_q: return 'q';
            case SDLK_w: return 'w';
            case SDLK_e: return 'e';
            case SDLK_r: return 'r';
            case SDLK_t: return 't';
            case SDLK_y: return 'y';
            case SDLK_u: return 'u';
            case SDLK_i: return 'i';
            case SDLK_o: return 'o';
            case SDLK_p: return 'p';
            case SDLK_LEFTBRACKET: return '[';
            case SDLK_RIGHTBRACKET: return ']';
            case SDLK_RETURN: return '\r';
            case SDLK_LCTRL: return 0x11;
            case SDLK_a: return 'a';
            case SDLK_s: return 's';
            case SDLK_d: return 'd';
            case SDLK_f: return 'f';
            case SDLK_g: return 'g';
            case SDLK_h: return 'h';
            case SDLK_j: return 'j';
            case SDLK_k: return 'k';
            case SDLK_l: return 'l';
            case SDLK_SEMICOLON: return ';';
            case SDLK_QUOTE: return '\'';
            case SDLK_BACKQUOTE: return '`';
            case SDLK_LSHIFT: return 0x10;
            case SDLK_BACKSLASH: return '\\';
            case SDLK_z: return 'z';
            case SDLK_x: return 'x';
            case SDLK_c: return 'c';
            case SDLK_v: return 'v';
            case SDLK_b: return 'b';
            case SDLK_n: return 'n';
            case SDLK_m: return 'm';
            case SDLK_COMMA: return ',';
            case SDLK_PERIOD: return '.';
            case SDLK_SLASH: return '/';
            case SDLK_KP_MULTIPLY: return '*';
            case SDLK_LALT: return 0x12;
            case SDLK_SPACE: return ' ';
            case SDLK_CAPSLOCK: return 0x14;
            case SDLK_F1: return 0x70;
            case SDLK_F2: return 0x71;
            case SDLK_F3: return 0x72;
            case SDLK_F4: return 0x73;
            case SDLK_F5: return 0x74;
            case SDLK_F6: return 0x75;
            case SDLK_F7: return 0x76;
            case SDLK_F8: return 0x77;
            case SDLK_F9: return 0x78;
            case SDLK_F10: return 0x79;
            case SDLK_NUMLOCKCLEAR: return 0x90;
            case SDLK_SCROLLLOCK: return 0x91;
            case SDLK_KP_7: return '7';
            case SDLK_KP_8: return '8';
            case SDLK_KP_9: return '9';
            case SDLK_KP_MINUS: return '-';
            case SDLK_KP_4: return '4';
            case SDLK_KP_5: return '5';
            case SDLK_KP_6: return '6';
            case SDLK_KP_PLUS: return '+';
            case SDLK_KP_1: return '1';
            case SDLK_KP_2: return '2';
            case SDLK_KP_3: return '3';
            case SDLK_KP_0: return '0';
            case SDLK_KP_PERIOD: return '.';
            case SDLK_PRINTSCREEN: return 0x2C;
            case SDLK_F11: return 0x7A;
            case SDLK_F12: return 0x7B;
            case SDLK_KP_ENTER: return '\r';
            case SDLK_RCTRL: return 0x11;
            case SDLK_KP_DIVIDE: return '/';
            case SDLK_RSHIFT: return 0x10;
            case SDLK_RALT: return 0x12;
            case SDLK_PAUSE: return 0x13;
            case SDLK_HOME: return 0x24;
            case SDLK_UP: return 0x26;
            case SDLK_PAGEUP: return 0x21;
            case SDLK_LEFT: return 0x25;
            case SDLK_RIGHT: return 0x27;
            case SDLK_END: return 0x23;
            case SDLK_DOWN: return 0x28;
            case SDLK_PAGEDOWN: return 0x22;
            case SDLK_INSERT: return 0x2D;
            case SDLK_DELETE: return 0x2E;
            case SDLK_LGUI: return 0x5B;
            case SDLK_RGUI: return 0x5C;
            case SDLK_MENU: return 0x5D;
            default: return 0x00;


            
        }
    }
    //<-end
    void translate(SDL_Event const &sdlEvent) {
        switch (sdlEvent.type) {
        case SDL_WINDOWEVENT:
            switch (sdlEvent.window.event) {

            case SDL_WINDOWEVENT_RESIZED:
                _shouldLayout = true;
                break;

            case SDL_WINDOWEVENT_EXPOSED:
                _dirty.pushBack(pixels().bound());
                break;
            }
            break;

        case SDL_KEYDOWN: {
			SDL_Keycode keyPressed = sdlEvent.key.keysym.sym;


            Events::KeyboardEvent uiEvent{
                .type = Events::KeyboardEvent::PRESS,
                .rune = Karm::Rune{mapSDLKeyToKey(keyPressed)},
            };
            event(uiEvent);
            break;
        }

        case SDL_KEYUP: {
            event<Events::TypedEvent>(*this, Events::KeyboardEvent::RELEASE);
            break;
        }

        case SDL_TEXTINPUT: {
            Str text = sdlEvent.text.text;
            for (u8 c : iterRunes(text)) {
                logInfo("typed: {c}", c);
                event<Events::TypedEvent>(*this, c);
            }
            break;
        }

        case SDL_MOUSEMOTION: {
            if (sdlEvent.motion.which == SDL_TOUCH_MOUSEID) {
                return;
            }

            Math::Vec2<i32> screenPos = {};
            SDL_GetGlobalMouseState(&screenPos.x, &screenPos.y);

            Events::Button buttons = Events::Button::NONE;
            buttons |= (sdlEvent.motion.state & SDL_BUTTON_LMASK) ? Events::Button::LEFT : Events::Button::NONE;
            buttons |= (sdlEvent.motion.state & SDL_BUTTON_MMASK) ? Events::Button::MIDDLE : Events::Button::NONE;
            buttons |= (sdlEvent.motion.state & SDL_BUTTON_RMASK) ? Events::Button::RIGHT : Events::Button::NONE;

            _lastMousePos = {sdlEvent.motion.x, sdlEvent.motion.y};

            event<Events::MouseEvent>(
                *this,
                Events::MouseEvent{
                    .type = Events::MouseEvent::MOVE,
                    .pos = _lastMousePos,
                    .delta = screenPos - _lastScreenMousePos,
                    .buttons = buttons,
                });

            _lastScreenMousePos = screenPos.cast<isize>();

            break;
        }

        case SDL_MOUSEBUTTONUP: {
            if (sdlEvent.motion.which == SDL_TOUCH_MOUSEID) {
                return;
            }

            Events::Button buttons = Events::Button::NONE;
            buttons |= (sdlEvent.motion.state & SDL_BUTTON_LMASK) ? Events::Button::LEFT : Events::Button::NONE;
            buttons |= (sdlEvent.motion.state & SDL_BUTTON_MMASK) ? Events::Button::MIDDLE : Events::Button::NONE;
            buttons |= (sdlEvent.motion.state & SDL_BUTTON_RMASK) ? Events::Button::RIGHT : Events::Button::NONE;

            Events::Button button = Events::Button::NONE;
            if (sdlEvent.button.button == SDL_BUTTON_LEFT) {
                button = Events::Button::LEFT;
            } else if (sdlEvent.button.button == SDL_BUTTON_RIGHT) {
                button = Events::Button::RIGHT;
            } else if (sdlEvent.button.button == SDL_BUTTON_MIDDLE) {
                button = Events::Button::MIDDLE;
            }

            event<Events::MouseEvent>(
                *this,
                Events::MouseEvent{
                    .type = Events::MouseEvent::RELEASE,
                    .pos = _lastMousePos,
                    .buttons = buttons,
                    .button = button,
                });
            break;
        }

        case SDL_MOUSEBUTTONDOWN: {
            if (sdlEvent.motion.which == SDL_TOUCH_MOUSEID) {
                return;
            }

            Events::Button buttons = Events::Button::NONE;
            buttons |= (sdlEvent.motion.state & SDL_BUTTON_LMASK) ? Events::Button::LEFT : Events::Button::NONE;
            buttons |= (sdlEvent.motion.state & SDL_BUTTON_MMASK) ? Events::Button::MIDDLE : Events::Button::NONE;
            buttons |= (sdlEvent.motion.state & SDL_BUTTON_RMASK) ? Events::Button::RIGHT : Events::Button::NONE;

            Events::Button button = Events::Button::NONE;
            if (sdlEvent.button.button == SDL_BUTTON_LEFT) {
                button = Events::Button::LEFT;
            } else if (sdlEvent.button.button == SDL_BUTTON_RIGHT) {
                button = Events::Button::RIGHT;
            } else if (sdlEvent.button.button == SDL_BUTTON_MIDDLE) {
                button = Events::Button::MIDDLE;
            }

            event<Events::MouseEvent>(
                *this,
                Events::MouseEvent{
                    .type = Events::MouseEvent::PRESS,
                    .pos = _lastMousePos,
                    .buttons = buttons,
                    .button = button,
                });
            break;
        }

        case SDL_MOUSEWHEEL: {
            if (sdlEvent.wheel.which == SDL_TOUCH_MOUSEID) {
                return;
            }

            event<Events::MouseEvent>(
                *this,
                Events::MouseEvent{
                    .type = Events::MouseEvent::SCROLL,
                    .pos = _lastMousePos,
                    .scroll = {
#if SDL_VERSION_ATLEAST(2, 0, 18)
                        sdlEvent.wheel.preciseX,
                        sdlEvent.wheel.preciseY,
#else
                        (f64)sdlEvent.wheel.x,
                        (f64)sdlEvent.wheel.y,
#endif
                    },
                });

            break;
        }

        case SDL_QUIT: {
            bubble<Events::ExitEvent>(*this, Ok());
            break;
        }

        default:
            break;
        }
    }

    void
    pump() override {
        SDL_Event e{};

        while (SDL_PollEvent(&e) != 0 and alive()) {
            translate(e);
        }
    }

    void wait(TimeSpan span) override {
        SDL_WaitEventTimeout(nullptr, span.toMSecs());
    }

    void bubble(Async::Event &e) override {
        if (e.is<Ui::DragEvent>()) {
            auto &dragEvent = e.unwrap<Ui::DragEvent>();
            if (dragEvent.type == Ui::DragEvent::START) {
                SDL_CaptureMouse(SDL_TRUE);
            } else if (dragEvent.type == Ui::DragEvent::END) {
                SDL_CaptureMouse(SDL_FALSE);
            } else if (dragEvent.type == Ui::DragEvent::DRAG) {
                Math::Vec2<i32> pos{};
                SDL_GetWindowPosition(_window, &pos.x, &pos.y);
                pos = pos + dragEvent.delta.cast<i32>();
                SDL_SetWindowPosition(_window, pos.x, pos.y);
            }
        }

        Ui::Host::bubble(e);
    }
};

Res<Strong<Karm::Ui::Host>> makeHost(Ui::Child root) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    auto size = root->size({700, 500}, Layout::Hint::MIN);

    SDL_Window *window = SDL_CreateWindow(
        "Application",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        size.width,
        size.height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE | SDL_WINDOW_BORDERLESS | SDL_WINDOW_UTILITY);

    if (not window) {
        return Error::other(SDL_GetError());
    }

    return Ok(makeStrong<SdlHost>(root, window));
}

} // namespace Karm::Ui::_Embed
