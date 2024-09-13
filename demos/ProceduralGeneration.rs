use consolegameengine::{ Colour, ConsoleGameEngine, Pixel, Rcode };

fn lehmer(seed: u32) -> u32 {
    let generator: u64 = seed as u64 + 0xE120FC15;
    let mut tmp = generator * 0x4A39B70D;

    let m: u32 = ((tmp >> 32) ^ tmp) as u32;
    tmp = m as u64 * 0x12FAD5C9;

    return ((tmp >> 32) ^ tmp) as u32;
}

fn wrap<T>(v: T, min: T, max: T) -> T
        where T: std::ops::Sub<Output = T> + std::ops::Add<Output = T> + std::ops::Rem<Output = T> + Copy {
    return v % (max - min) + min;
}

fn main() {
    let mut app = ConsoleGameEngine::new();

    let mut pan_started = false;

    let mut pan_start_x = 0;
    let mut pan_start_y = 0;

    let mut offset_x = 0;
    let mut offset_y = 0;

    if let Rcode::Ok = app.construct_console(256, 240, 4, 4, "Procedural Generation") {
        while let Rcode::Ok = app.update() {
            let mouse_x = app.mouse_x();
            let mouse_y = app.mouse_y();

            if app.get_mouse(0).pressed {
                pan_start_x = mouse_x;
                pan_start_y = mouse_y;
                pan_started = true;
            }

            if app.get_mouse(0).released {
                pan_started = false;
            }

            if pan_started {
                offset_x += pan_start_x - mouse_x;
                offset_y += pan_start_y - mouse_y;

                pan_start_x = mouse_x;
                pan_start_y = mouse_y;
            }

            app.clear(Pixel::Solid as u16, Colour::Black as u16);

            app.draw_string(1, 1, format!("({mouse_x}, {mouse_y})"), Colour::FgRed as u16);

            for i in 0..app.screen_width() as i32 {
                for j in 0..app.screen_height() as i32{
                    let seed = (j + offset_y) << 16 | (i + offset_x);
                    let rand = lehmer(seed as u32);

                    if rand % 2048 < 4 {
                        app.fill_circle(
                            i, j,
                            wrap(rand as i32, 3, 8),
                            Pixel::Solid as u16,
                            wrap(rand, 1, 15) as u16
                        );
                    }
                }
            }
        }
    }
}
