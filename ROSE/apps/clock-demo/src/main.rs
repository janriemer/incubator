mod app;

extern crate chrono;
extern crate sdl2;
extern crate sdlstate;
extern crate stencil;

use stencil::stencil::Stencil;
use stencil::types::{Dimension, Point, Rect, Unit};

use sdl2::event::{Event, WindowEvent};
use sdl2::libc;
use sdl2::mouse::MouseButton;
use sdlstate::SdlState;

use app::demo_init;

const W: Dimension = 320;
const H: Dimension = 200;

/// The main entry point is ultimately responsible for driving the entire application environment.
/// It creates the SDL (or other platform-specific) frame buffer surface on which we ultimately
/// render our desktop environment.  It then integrates with the SDL (or platform-specific) event
/// sources to generate what the clock application considers as low-level events: button click
/// events, mouse movement events, and so forth.
///
/// From this event loop's perspective, the entire environment is comprised of just two functions:
/// [[demo_init]] and [[ClockState::demo_tick]].  The former is responsible for configuring the application
/// environment, including painting the initial desktop environment.  The latter is responsible for
/// handling subsequent events.
///
/// Note that this event loop considers two sources of events: the application itself and SDL.
/// For this reason, application-generated events are called *commands* (hence, `enum Cmd`), since
/// they tell `main` what to do next.  Note that [[demo_tick]] accepts a command as an input as
/// well, indicating the most recently processed command.  This is sometimes useful for multi-step
/// command processing.
fn main() {
    // Create our SDL bindings and unpack our toybox.
    let mut sdl = SdlState::new("Clock Demo", W as u32, H as u32);
    let event_subsystem = sdl.context.event().unwrap();
    let timer_subsystem = sdl.context.timer().unwrap();

    // Gain access to our event queue.
    let mut event_pump = sdl.context.event_pump().unwrap();

    // Create a 500ms timer that generates a TimerTick event when it fires.
    //
    // To do this, we must first register our TimerTick event ID.
    let timer_tick = unsafe { event_subsystem.register_event().unwrap() };
    const PERIOD: u32 = 500; // milliseconds
    let _timer = timer_subsystem.add_timer(
        PERIOD,
        Box::new(|| {
            let _ = event_subsystem
                .push_event(Event::User {
                    timestamp: 0,
                    window_id: 0,
                    type_: timer_tick,
                    code: 0,
                    data1: 0 as *mut libc::c_void,
                    data2: 0 as *mut libc::c_void,
                })
                .unwrap();
            PERIOD
        }),
    );

    // Create our desktop stencil.
    let mut desktop = Stencil::new_with_dimensions(W, H);

    // Enter the main loop for the clock.
    // We start by initializing the clock.
    // Then, delegate to the timer's event handler for every event we receive.
    let mut clock_state = demo_init(&mut desktop);
    'main_event_loop: loop {
        for event in event_pump.wait_iter() {
            let command = match event {
                Event::Quit { .. } => HostAction::Quit,
                Event::Window { win_event: we, .. } if we == WindowEvent::Exposed => {
                    HostAction::Repaint(((0, 0), (W, H)))
                }
                Event::MouseButtonUp {
                    mouse_btn: b, x, y, ..
                } => clock_state.demo_tick(
                    &mut desktop,
                    Cmd::ButtonUp {
                        button: button_for(b),
                        at: (x as Unit, y as Unit),
                    },
                ),
                Event::MouseButtonDown {
                    mouse_btn: b, x, y, ..
                } => clock_state.demo_tick(
                    &mut desktop,
                    Cmd::ButtonDown {
                        button: button_for(b),
                        at: (x as Unit, y as Unit),
                    },
                ),
                Event::User { type_: t, .. } if t == timer_tick => {
                    clock_state.demo_tick(&mut desktop, Cmd::TimerTick)
                }
                _ => HostAction::None,
            };

            match command {
                HostAction::Repaint(_) => repaint(&mut desktop, &mut sdl),
                HostAction::Quit => break 'main_event_loop,
                _ => (),
            }
        }
    }
}

/// Translate SDL-specific mouse button identity to something more convenient to work with.
fn button_for(b: MouseButton) -> usize {
    match b {
        MouseButton::Left => 1,
        MouseButton::Right => 2,
        MouseButton::Middle => 3,
        _ => 0,
    }
}

pub enum HostAction {
    None,
    Quit,
    Repaint(Rect),
}

pub enum Cmd {
    WaitEvent,
    ButtonUp { button: usize, at: Point },
    ButtonDown { button: usize, at: Point },
    TimerTick,
}

/// Repaint the screen and make it visible to the human operator.
///
/// This function performs color-expansion and/or retiling as appropriate to render the contents of
/// the `desktop` stencil to the display.
fn repaint(desktop: &mut Stencil, sdl: &mut SdlState) {
    // Sadly, because of how SDL2 works with modern video equipment,
    // we must refresh the entire surface; we can't be clever and just
    // refresh a subset of a surface.  Therefore, the `r` parameter is
    // unused.
    let ((left, top), (right, bottom)) = ((0, 0), desktop.dimensions);
    let (left, top) = (left as usize, top as usize);
    let (right, bottom) = (right as usize, bottom as usize);
    let width = right - left;
    let height = bottom - top;

    sdl.paint_with(|ctx| {
        ctx.paste_stamp_be(
            (left, top),
            (width, height),
            desktop.get_span(),
            (left, top),
            desktop.borrow_bits(),
        );
    });
}
