use win32console::console::{WinConsole, ConsoleMode};
use win32console::input::InputRecord::{FocusEvent, KeyEvent, MouseEvent, WindowBufferSizeEvent};
use win32console::input::{Coord, EventFlags};
use win32console::structs::console_font_info_ex::ConsoleFontInfoEx;
use win32console::structs::small_rect::SmallRect;
use winapi::shared::minwindef::DWORD;
use winapi::um::wincon::WriteConsoleOutputW;
use winapi::um::playsoundapi::{PlaySoundW, SND_ASYNC, SND_LOOP, SND_FILENAME};
use winapi::um::wincontypes::{CHAR_INFO, CHAR_INFO_Char, COORD};
use std::fs::File;
use std::io::{BufReader, BufWriter, Read, Write};
use std::time::Instant;

#[derive(Clone, Copy)]
#[repr(u16)]
pub enum Colour {
    Black,
    FgDarkBlue,
    FgDarkGreen,
    FgDarkCyan,
    FgDarkRed,
    FgDarkMagenta,
    FgDarkYellow,
    FgGrey,
    FgDarkGrey,
    FgBlue,
    FgGreen,
    FgCyan,
    FgRed,
    FgMagenta,
    FgYellow,
    FgWhite,
    BgDarkBlue = 0x0010,
    BgDarkGreen = 0x0020,
    BgDarkCyan = 0x0030,
    BgDarkRed = 0x0040,
    BgDarkMagenta = 0x0050,
    BgDarkYellow = 0x0060,
    BgGrey = 0x0070,
    BgDarkGrey = 0x0080,
    BgBlue = 0x0090,
    BgGreen = 0x00A0,
    BgCyan = 0x00B0,
    BgRed = 0x00C0,
    BgMagenta = 0x00D0,
    BgYellow = 0x00E0,
    BgWhite = 0x00F0
}

#[derive(Clone, Copy)]
#[repr(u16)]
pub enum Pixel {
    Solid = 0x2588,
    ThreeQuarters = 0x2593,
    Half = 0x2592,
    Quarter = 0x2591
}

pub enum CommonLvb {
    GridHorizontal = 0x400,
    GridLVertical = 0x0800,
    GridRVertical = 0x1000,
    Underscore = 0x8000
}

#[derive(Clone, Copy)]
pub struct KeyState {
    pub held: bool,
    pub released: bool,
    pub pressed: bool
}

pub struct Sprite {
    pub width: usize,
    pub height: usize,

    pub glyphs: Vec<u16>,
    pub colours: Vec<u16>
}

pub enum Rcode {
    Ok,
    InvalidScreenSize,
    InvalidScreenBuffer,
    InvalidFont,
    InvalidConsoleMode,
    InvalidScreenInfo
}

pub struct ConsoleGameEngine {
    app_name: String,
    font_name: String,

    screen: Vec<CHAR_INFO>,
    console_out: WinConsole,
    console_in: WinConsole,

    window_rect: SmallRect,

    keys: [KeyState; 256],
    mouse: [KeyState; 5],

    keys_old_state: [bool; 256],
    keys_new_state: [bool; 256],

    mouse_old_state: [bool; 5],
    mouse_new_state: [bool; 5],

    mouse_x: i32,
    mouse_y: i32,

    screen_width: usize,
    screen_height: usize,

    font_width: usize,
    font_height: usize,

    delta_time: f32,

    is_focused: bool,

    time_last: Instant
}

fn read_i32(reader: &mut BufReader<File>) -> i32 {
    let mut buf = [u8::from(0); 4];
    let _ = reader.read_exact(&mut buf);
    return i32::from_be_bytes(buf);
}

fn read_u16(reader: &mut BufReader<File>) -> u16 {
    let mut buf = [u8::from(0); 2];
    let _ = reader.read_exact(&mut buf);
    return u16::from_be_bytes(buf);
}

fn read_vec_u16(reader: &mut BufReader<File>, out: &mut Vec<u16>) {
    for val in out {
        *val = read_u16(reader);
    }
}

fn write_i32(writer: &mut BufWriter<File>, value: i32) {
    let _ = writer.write(&value.to_be_bytes());
}

fn write_u16(writer: &mut BufWriter<File>, value: u16) {
    let _ = writer.write(&value.to_be_bytes());
}

fn write_vec_u16(writer: &mut BufWriter<File>, out: &Vec<u16>) {
    for val in out.iter() {
        write_u16(writer, *val);
    }
}

fn write_with_u16bytes(string: String, bytes: &mut [u16], bytes_count: usize) {
    let mut buf: Vec<u16> = string.encode_utf16().collect();
    buf.push(0);

    for i in 0..bytes_count.min(buf.len()) {
        bytes[i] = buf[i];
    }
}

fn new_char_info(char: u16, attributes: u16) -> CHAR_INFO {
    let mut uchar = CHAR_INFO_Char::default();
	unsafe { *uchar.UnicodeChar_mut() = char };

    CHAR_INFO {
        Char: uchar,
        Attributes: attributes
    }
}

impl From<Colour> for u16 {
    fn from(value: Colour) -> Self {
        value as u16
    }
}

impl From<Pixel> for u16 {
    fn from(value: Pixel) -> Self {
        value as u16
    }
}

impl KeyState {
    pub fn new(held: bool, released: bool , pressed: bool) -> Self {
        Self {
            held: held,
            released: released,
            pressed: pressed
        }
    }
}

impl Sprite {
    pub fn new() -> Self {
        Self {
            width: 0,
            height: 0,
            glyphs: vec![],
            colours: vec![]
        }
    }

    pub fn create(width: usize, height: usize) -> Self {
        Self {
            width: width,
            height: height,
            glyphs: vec![0x2588; width * height],
            colours: vec![0; width * height]
        }
    }

    pub fn from_file(file_name: &str) -> Self {
        let mut buf = BufReader::new(File::open(file_name).expect(format!("Can't open {file_name}").as_str()));

        let width: usize = read_i32(&mut buf).try_into().unwrap();
        let height: usize = read_i32(&mut buf).try_into().unwrap();

        let mut glyphs: Vec<u16> = vec![0; width * height];
        let mut colours: Vec<u16> = vec![0; width * height];

        read_vec_u16(&mut buf, &mut glyphs);
        read_vec_u16(&mut buf, &mut colours);
        
        Self {
            width: width,
            height: height,
            glyphs: glyphs,
            colours: colours
        }
    }

    pub fn save(&self, file_name: &str) {
        let mut buf = BufWriter::new(File::open(file_name).expect(format!("Can't open {file_name}").as_str()));

        write_i32(&mut buf, self.width as i32);
        write_i32(&mut buf, self.height as i32);

        write_vec_u16(&mut buf, &self.glyphs);
        write_vec_u16(&mut buf, &self.colours);
    }

    pub fn set_glyph(&mut self, x: i32, y: i32, glyph: u16) {
        if x >= 0 && y >= 0 && x < self.width as i32 && y < self.height as i32 {
            self.glyphs[y as usize * self.width + x as usize] = glyph;
        }
    }

    pub fn set_colour(&mut self, x: i32, y: i32, colour: u16) {
        if x >= 0 && y >= 0 && x < self.width as i32 && y < self.height as i32 {
            self.colours[y as usize * self.width + x as usize] = colour;
        }
    }

    pub fn get_glyph(&self, x: i32, y: i32) -> u16 {
        if x >= 0 && y >= 0 && x < self.width as i32 && y < self.height as i32 {
            return self.glyphs[y as usize * self.width + x as usize] as u16
        }

        0x2588 // Pixel::Solid
    }

    pub fn get_colour(&self, x: i32, y: i32) -> u16 {
        if x >= 0 && y >= 0 && x < self.width as i32 && y < self.height as i32 {
            return self.colours[y as usize * self.width + x as usize]
        }

        Colour::Black as u16
    }
}

pub fn make_sound(file_name: &str, looped: bool) -> bool {
    let mut flags: DWORD = SND_ASYNC | SND_FILENAME;
    if looped { flags |= SND_LOOP; }

    let buffer = match std::str::from_utf8(file_name.as_bytes()) {
        Ok(string) => {
            string.encode_utf16().collect::<Vec<u16>>()
        },
        Err(_) => { return false }
    };

    unsafe { PlaySoundW(buffer.as_ptr(), std::mem::zeroed(), flags) == 1 }
}

impl ConsoleGameEngine {
    pub fn new() -> Self {
        Self {
            console_out: WinConsole::output(),
            console_in: WinConsole::input(),

            app_name: String::from("Undefined"),
            font_name: String::from("Consolas"),

            delta_time: 0.0,

            screen: vec![],

            window_rect: SmallRect::default(),

            keys: [KeyState::new(false, false, false); 256],
            keys_new_state: [false; 256],
            keys_old_state: [false; 256],

            mouse: [KeyState::new(false, false, false); 5],
            mouse_new_state: [false; 5],
            mouse_old_state: [false; 5],

            mouse_x: 0,
            mouse_y: 0,

            screen_width: 0,
            screen_height: 0,

            font_width: 0,
            font_height: 0,

            is_focused: false,

            time_last: Instant::now()
        }
    }

    pub fn construct_console(&mut self, screen_width: usize, screen_height: usize, font_width: usize, font_height: usize, title: &str) -> Rcode {
        if screen_width == 0 || screen_height == 0 || font_width == 0 || font_height == 0 {
            return Rcode::InvalidScreenSize;
        }
        
        self.screen_width = screen_width;
        self.screen_height = screen_height;

        self.font_width = font_width;
        self.font_height = font_height;

        self.app_name = String::from(title);

        self.window_rect.left = 0;
        self.window_rect.top = 0;
        self.window_rect.right = 1;
        self.window_rect.bottom = 1;

        if let Err(_) = self.console_out.set_window_info(true, &self.window_rect) {
            return Rcode::InvalidScreenInfo;
        }

        if let Err(_) = self.console_out.set_screen_buffer_size(Coord::new(screen_width as i16, screen_height as i16)) {
            return Rcode::InvalidScreenSize;
        }

        let mut font_name = [0; 32];
        write_with_u16bytes(self.font_name.clone(), &mut font_name, 31);

        let cfi = ConsoleFontInfoEx {
            size: size_of::<ConsoleFontInfoEx>() as u32,
            font_index: 0,
            font_size: Coord::new(font_width as i16, font_height as i16),
            font_family: 0, // FF_DONTCARE
            font_weight: 400, // FW_NORMAL
            face_name: font_name
        };

        if let Err(_) = self.console_out.set_font_ex(cfi, false) {
            return Rcode::InvalidFont
        }

        if let Err(_) = self.console_in.set_mode(ConsoleMode::ENABLE_EXTENDED_FLAGS | ConsoleMode::ENABLE_WINDOW_INPUT | ConsoleMode::ENABLE_MOUSE_INPUT) {
            return Rcode::InvalidConsoleMode
        }

        if let Ok(csbi) = self.console_out.get_screen_buffer_info() {
            if (screen_height as i16 > csbi.maximum_window_size.y) || (screen_width as i16 > csbi.maximum_window_size.x) {
                return Rcode::InvalidScreenSize
            }
        } else {
            return Rcode::InvalidScreenInfo
        }

        self.window_rect.left = 0;
        self.window_rect.top = 0;
        self.window_rect.right = screen_width as i16 - 1;
        self.window_rect.bottom = screen_height as i16 - 1;

        if let Err(_) = self.console_out.set_window_info(true, &self.window_rect) {
            return Rcode::InvalidScreenInfo
        }

        self.screen = vec![new_char_info(0x2588 as u16, 0); screen_width * screen_height];
        self.time_last = Instant::now();

        let _ = self.console_out.set_cursor_position(Coord::new(0, 0));

        Rcode::Ok
    }

    pub fn update(&mut self) -> Rcode {
        let now = Instant::now();

        self.delta_time = (now - self.time_last).as_secs_f32();
        self.time_last = now;

        let title = format!("github.com/defini7 - Rust Console Game Engine - {} - FPS: {}", self.app_name, (1.0 / self.delta_time) as u32);

        let _ = WinConsole::set_title(title.as_str());

        let events = self.console_in.get_number_of_input_events().unwrap();
        let input = self.console_in.read_input_n(events).unwrap();

        for record in input {
            match record {
                FocusEvent(evt) => {
                    self.is_focused = evt.set_focus;
                },

                KeyEvent(evt) => {
                    self.keys_new_state[evt.virtual_key_code as usize] = evt.key_down;
                },

                WindowBufferSizeEvent(evt) => {
                    self.screen_width = evt.size.x as usize;
                    self.screen_height = evt.size.y as usize;
                },

                MouseEvent(evt) => {
                    match evt.event_flags {
                        EventFlags::MouseMoved => {
                            self.mouse_x = evt.mouse_position.x as i32;
                            self.mouse_y = evt.mouse_position.y as i32;
                        },

                        EventFlags::PressOrRelease => {
                            for m in 0..5 {
                                self.mouse_new_state[m] = (evt.button_state.get_state() & (1 << m)) > 0;
                            }
                        },

                        _ => {}
                    }
                },

                _ => {}
            }
        }

        for i in 0..256 {
            self.keys[i].pressed = false;
            self.keys[i].released = false;

            if self.keys_new_state[i] != self.keys_old_state[i] {
                if self.keys_new_state[i] {
                    self.keys[i].released = true;
                    self.keys[i].held = true;
                } else {
                    self.keys[i].pressed = !self.keys[i].held;
                    self.keys[i].held = true;
                }
            }

            self.keys_old_state[i] = self.keys_new_state[i];
        }

        for i in 0..5 {
            self.mouse[i].pressed = false;
            self.mouse[i].released = false;

            if self.mouse_new_state[i] != self.mouse_old_state[i] {
                if self.mouse_new_state[i] {
                    self.mouse[i].pressed = true;
                    self.mouse[i].held = true;
                } else {
                    self.mouse[i].released = true;
                    self.mouse[i].held = false;
                }
            }

            self.mouse_old_state[i] = self.mouse_new_state[i];
        }

        unsafe {
            if WriteConsoleOutputW(
                **self.console_out.get_handle(),
                self.screen.as_ptr(),
                COORD { X: self.screen_width as i16, Y: self.screen_height as i16 },
                COORD { X: 0, Y: 0 },
                &mut self.window_rect.into()
            ) == 0 {
                return Rcode::InvalidScreenBuffer
            }
        }

        Rcode::Ok
    }

    pub fn draw(&mut self, x: i32, y: i32, glyph: u16, col: u16) {
        if x >= 0 && x < self.screen_width as i32 && y >= 0 && y < self.screen_height as i32 {
            let i = (y * self.screen_width as i32 + x) as usize;

            unsafe { *self.screen[i].Char.UnicodeChar_mut() = glyph as u16; }
            self.screen[i].Attributes = col as u16;
        }
    }

    pub fn fill_rectangle(&mut self, x: i32, y: i32, size_x: i32, size_y: i32, glyph: u16, col: u16) {
        for i in 0..size_x {
            for j in 0..size_y {
                self.draw(x + i, y + j, glyph, col);
            }
        }
    }
    
    pub fn draw_circle(&mut self, x: i32, y: i32, radius: i32, glyph: u16, col: u16) {
        if radius <= 0 { return }

        let mut x1 = 0;
        let mut y1 = radius;
        let mut p = 3 - 2 * radius;

        while y1 >= x1 {
            self.draw(x - x1, y - y1, glyph, col);
            self.draw(x - y1, y - x1, glyph, col);
            self.draw(x + y1, y - x1, glyph, col);
            self.draw(x + x1, y - y1, glyph, col);
            self.draw(x - x1, y + y1, glyph, col);
            self.draw(x - y1, y + x1, glyph, col);
            self.draw(x + y1, y + x1, glyph, col);
            self.draw(x + x1, y + y1, glyph, col);

            if p < 0 {
                p += 4 * x1 + 6;
                x1 += 1;
            } else {
                p += 4 * (x1 - y1) + 10;
                x1 += 1;
                y1 -= 1;
            }
        }
    }

    pub fn fill_circle(&mut self, x: i32, y: i32, radius: i32, glyph: u16, col: u16) {
        if radius <= 0 { return }

        let mut x1 = 0;
        let mut y1 = radius;
        let mut p = 3 - 2 * radius;

        let mut draw_line = |sx: i32, ex: i32, ny: i32| {
            for i in sx..(ex + 1) {
                self.draw(i, ny, glyph, col);
            }
        };

        while y1 >= x1 {
            draw_line(x - x1, x + x1, y - y1);
            draw_line(x - y1, x + y1, y - x1);
            draw_line(x - x1, x + x1, y + y1);
            draw_line(x - y1, x + y1, y + x1);

            if p < 0 {
                p += 4 * x1 + 6;
                x1 += 1;
            } else {
                p += 4 * (x1 - y1) + 10;
                x1 += 1;
                y1 -= 1;
            }
        }
    }

    pub fn draw_line(&mut self, x1: i32, y1: i32, x2: i32, y2: i32, glyph: u16, col: u16) {
        let mut x: i32;
        let mut y: i32;

        let xe: i32;
        let ye: i32;

        let dx = x2 - x1;
        let dy = y2 - y1;

        let dx1 = dx.abs();
        let dy1 = dy.abs();

        let mut px = 2 * dy1 - dx1;
        let mut py = 2 * dx1 - dy1;

        if dy1 <= dx1 {
            if dx >= 0 {
                x = x1;
                y = y1;
                xe = x2;
            } else {
                x = x2;
                y = y2;
                xe = x1;
            }

            self.draw(x, y, glyph, col);

            while x < xe {
                x += 1;

                if px < 0 {
                    px += 2 * dy1;
                } else {
                    y += if (dx < 0 && dy < 0) || (dx > 0 && dy > 0) { 1 } else { -1 };
                    px += 2 * (dy1 - dx1);
                }

                self.draw(x, y, glyph, col);
            }
        } else {
            if dy >= 0 {
                x = x1;
                y = y1;
                ye = y2;
            } else {
                x = x2;
                y = y2;
                ye = y1;
            }

            self.draw(x, y, glyph, col);

            while y < ye {
                y += 1;

                if py <= 0 {
                    py += 2 * dx1;
                } else {
                    x += if (dx < 0 && dy < 0) || (dx > 0 && dy > 0) { 1 } else { -1 };
                    py += 2 * (dx1 - dy1);
                }

                self.draw(x, y, glyph, col);
            }
        }
    }

    pub fn draw_triangle(&mut self, x1: i32, y1: i32, x2: i32, y2: i32, x3: i32, y3: i32, glyph: u16, col: u16) {
        self.draw_line(x1, y1, x2, y2, glyph, col);
        self.draw_line(x2, y2, x3, y3, glyph, col);
        self.draw_line(x3, y3, x1, y1, glyph, col);
    }

    pub fn fill_triangle(&mut self, mut x1: i32, mut y1: i32, mut x2: i32, mut y2: i32, mut x3: i32, mut y3: i32, glyph: u16, col: u16) {
        let mut changed1 = false;
        let mut changed2 = false;
        
        // sort vertices
        if y1 > y2 {
            (y1, y2) = (y2, y1);
            (x1, x2) = (x2, x1);
        }

        if y1 > y3 {
            (y1, y3) = (y3, y1);
            (x1, x3) = (x3, x1);
        }

        if y2 > y3 {
            (y2, y3) = (y3, y2);
            (x2, x3) = (x3, x2);
        }
        
        let mut t1x = x1;
        let mut t2x = x1;
        let mut y = y1;
        let mut dx1 = x2 - x1;

        let signx1 = if dx1 < 0 {
            dx1 = -dx1;
            -1
        } else {
            1
        };

        let mut dy1 = y2 - y1;
        let mut dx2 = x3 - x1;

        let signx2 = if dx2 < 0 {
            dx2 = -dx2;
            -1
        } else {
            1
        };

        let mut dy2 = y3 - y1;
        
        if dy1 > dx1 {
            (dx1, dy1) = (dy1, dx1);
            changed1 = true;
        }
        
        if dy2 > dx2 {
            (dx2, dy2) = (dy2, dx2);
            changed2 = true;
        }
        
        let mut e2 = dx2 >> 1;

        if y1 != y2 {
            let mut e1 = dx1 >> 1;
            
            for mut i in 0..dx1 {
                let mut t1xp = 0;
                let mut t2xp = 0;

                let (mut minx, mut maxx) = if t1x < t2x {
                    (t1x, t2x)
                } else {
                    (t2x, t1x)
                };

                'first_line_1: while i < dx1 {
                    i += 1;
                    e1 += dy1;

                    while e1 >= dx1 {
                        e1 -= dx1;

                        if changed1 {
                            t1xp = signx1;
                        } else {
                            break 'first_line_1;
                        }
                    }

                    if changed1 {
                        break 'first_line_1;
                    } else {
                        t1x += signx1;
                    }
                }
                
                'second_line_1: loop {
                    e2 += dy2;

                    while e2 >= dx2 {
                        e2 -= dx2;

                        if changed2 {
                            t2xp = signx2;
                        } else {
                            break 'second_line_1;
                        }
                    }

                    if changed2 {
                        break 'second_line_1;
                    } else {
                        t2x += signx2;
                    }
                }
                
                if minx > t1x { minx = t1x; }
                if minx > t2x { minx = t2x; }
                if maxx < t1x { maxx = t1x; }
                if maxx < t2x { maxx = t2x; }

                for j in minx..=maxx {
                    self.draw(j, y, glyph, col);
                }
                
                // now increase y
                if !changed1 {
                    t1x += signx1;
                }

                t1x += t1xp;

                if !changed2 {
                    t2x += signx2;
                }

                t2x += t2xp;
                y += 1;
                
                if y == y2 {
                    break;
                }
            }
        }
        
        dx1 = x3 - x2;

        let signx1 = if dx1 < 0 {
            dx1 = -dx1;
            -1
        } else {
            1
        };

        dy1 = y3 - y2;
        t1x = x2;
        
        if dy1 > dx1 {
            (dy1, dx1) = (dx1, dy1);
            changed1 = true;
        } else {
            changed1 = false;
        }

        let mut e1 = dx1 >> 1;
        
        for mut i in 0..=dx1 {
            let mut t1xp = 0;
            let mut t2xp = 0;

            let (mut minx, mut maxx) = if t1x < t2x {
                (t1x, t2x)
            } else {
                (t2x, t1x)
            };

            'first_line_2: while i < dx1 {
                e1 += dy1;

                while e1 >= dx1 {
                    e1 -= dx1;

                    if changed1 {
                        t1xp = signx1;
                        break;
                    } else {
                        break 'first_line_2;
                    }
                }

                if changed1 {
                    break 'first_line_2;
                } else {
                    t1x += signx1;
                }

                if i < dx1 {
                    i += 1;
                }
            }
            
            'second_line_2: while t2x != x3 {
                e2 += dy2;

                while e2 >= dx2 {
                    e2 -= dx2;
                    if changed2 {
                        t2xp = signx2;
                    } else {
                        break 'second_line_2;
                    }
                }

                if changed2 {
                    break 'second_line_2;
                } else {
                    t2x += signx2;
                }
            }
            
            if minx > t1x { minx = t1x; }
            if minx > t2x { minx = t2x; }
            if maxx < t1x { maxx = t1x; }
            if maxx < t2x { maxx = t2x; }

            for j in minx..=maxx {
                self.draw(j, y, glyph, col);
            }
            
            // now increase y
            if !changed1 {
                t1x += signx1;
            }

            t1x += t1xp;

            if !changed2 {
                t2x += signx2;
            }

            t2x += t2xp;
            y += 1;

            if y > y3 {
                return;
            }
        }
    }

    pub fn draw_rectangle(&mut self, x: i32, y: i32, w: i32, h: i32, glyph: u16, col: u16) {
        self.draw_line(x, y, x + w, y, glyph, col);
		self.draw_line(x + w, y, x + w, y + h, glyph, col);
		self.draw_line(x + w, y + h, x, y + h, glyph, col);
		self.draw_line(x, y + h, x, y, glyph, col);
    }

    pub fn draw_sprite(&mut self, x: i32, y: i32, sprite: &Sprite, alpha: bool) {
        for i in 0..sprite.width as i32 {
            for j in 0..sprite.height as i32 {
                let glyph = sprite.get_glyph(i, j);

                if alpha && glyph != ' ' as u16 {
                    self.draw(x + i, y + i, glyph, sprite.get_colour(i, j));
                }
            }
        }
    }

    pub fn draw_partial_sprite(&mut self, x: i32, y: i32, fx: i32, fy: i32, fw: i32, fh: i32, sprite: &Sprite, alpha: bool) {
        if fw > sprite.width as i32 || fh > sprite.height as i32 {
            return
        }

        for i in 0..fw as i32 {
            for j in 0..fh as i32 {
                let glyph = sprite.get_glyph(i, j);

                if alpha && glyph != ' ' as u16 {
                    self.draw(x + i, y + j, glyph, sprite.get_colour(fx + i, fy + j));
                }
            }
        }
    }

    pub fn draw_wire_frame_model(&mut self, mut model: Vec<(f32, f32)>, x: f32, y: f32, rotation: f32, scale: f32, glyph: u16, col: u16) {
        for p in model.iter_mut() {
            p.0 = (p.0 * rotation.cos() - p.1 * rotation.sin()) * scale + x;
            p.1 = (p.0 * rotation.sin() + p.1 * rotation.cos()) * scale + y;
        }

        for i in 0..=model.len() {
            let j = i + 1;

            self.draw_line(
                model[i % model.len()].0 as i32,
                model[i % model.len()].1 as i32,
                model[j % model.len()].0 as i32,
                model[j % model.len()].1 as i32,
                glyph, col
            );
        }
    }
    
    pub fn draw_string(&mut self, x: i32, y: i32, text: String, col: u16) {
        let data = text.as_bytes();

        for i in 0..text.len() {
            let j = y as usize * self.screen_width + x as usize + i;

            unsafe { *self.screen[j].Char.UnicodeChar_mut() = data[i].into(); };
            self.screen[j].Attributes = col;
        }
    }

    pub fn clear(&mut self, glyph: u16, col: u16) {
        self.fill_rectangle(0, 0, self.screen_width as i32, self.screen_height as i32, glyph, col);
    }

    pub fn get_mouse_x(&self) -> i32 { self.mouse_x }
    pub fn get_mouse_y(&self) -> i32 { self.mouse_y }

    pub fn get_mouse(&self, button: usize) -> KeyState { self.mouse[button] }
    pub fn get_key(&self, key: usize) -> KeyState { self.keys[key] }

    pub fn screen_width(&self) -> usize { self.screen_width }
    pub fn screen_height(&self) -> usize { self.screen_height }

}
