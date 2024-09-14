use consolegameengine::{ Colour, ConsoleGameEngine, Rcode };
use escapi;

const VARIATES: f32 = 16.0;

fn main() -> Result<(), i32> {
    let mut app = ConsoleGameEngine::new();

    if !matches!(app.construct_console(160, 120, 8, 8, "Web Camera"), Rcode::Ok) {
        return Err(0);
    }

    let device = escapi::init(0, app.screen_width() as u32, app.screen_height() as u32, 15).expect("Could not initialize the camera");

    while let Rcode::Ok = app.update() {
        let capture = device.capture().expect("Could not capture an image");
        let capture_size = device.capture_width() * device.capture_height();

        for _ in 0..15 {
            for i in 0..capture_size {
                let x = (i % device.capture_width()) as i32;
                let y = (i / device.capture_width()) as i32;

                let r = capture[i as usize * 4 + 2] as f32;
                let g = capture[i as usize * 4 + 1] as f32;
                let b = capture[i as usize * 4 + 0] as f32;

                let luminance = (0.2987 * r + 0.587 * g + 0.114 * b) / 255.0 * VARIATES;

                let char = luminance as u16 + 65;

                app.draw(x, y, char, Colour::FgWhite as u16);
            }
        }
    }

    return Ok(());
}
