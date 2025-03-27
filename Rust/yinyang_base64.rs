use std::error::Error;

const BASE64_CHARS: &str = "陰陽天干地支五行金木水火土甲乙丙丁戊己庚辛壬癸子丑寅卯辰巳午未申酉戌亥立春驚蟄清明夏芒種小暑秋白露寒冬大雪雨分穀滿至處霜降生剋沖";
const SINGLE_PAD: &str = "宮";
const DOUBLE_PAD: &str = "星宿";

fn index_of_base64_chars(c: char) -> u8 {
    return BASE64_CHARS.chars().position(|_c| _c == c).unwrap_or(255) as u8;
}

pub fn get_string(bytes: &[u8]) -> String {
    let length = bytes.len();
    if length == 0 {
        return String::new();
    }

    let mut padding = length % 3 as usize;
    if padding > 0 {
        padding = 3 - padding;
    }
    let blocks = (length - 1) / 3 + 1;

    let mut s = String::with_capacity(blocks * 4 * 3); // * 3 for kanji character size

    for i in 0..blocks {
        let final_block = i == blocks - 1;
        let mut pad2 = false;
        let mut pad1 = false;
        if final_block {
            pad2 = padding == 2;
            pad1 = padding > 0;
        }

        let index = i * 3;
        let b1 = bytes[index];
        let b2 = if pad2 { 0 } else { bytes[index + 1] };
        let b3 = if pad1 { 0 } else { bytes[index + 2] };

        let temp1 = (b1 & 0xFC) >> 2;
        let temp2 = ((b1 & 0x03) << 4) | ((b2 & 0xF0) >> 4);
        let temp3 = ((b2 & 0x0F) << 2) | ((b3 & 0xC0) >> 6);
        let temp4 = b3 & 0x3F;

        s.push(BASE64_CHARS.chars().nth(temp1 as usize).unwrap());
        s.push(BASE64_CHARS.chars().nth(temp2 as usize).unwrap());
        if pad1 && pad2 {
            s.push(DOUBLE_PAD.chars().nth(0).unwrap());
            s.push(DOUBLE_PAD.chars().nth(1).unwrap());
        } else if pad1 {
            s.push(BASE64_CHARS.chars().nth(temp3 as usize).unwrap());
            s.push(SINGLE_PAD.chars().nth(0).unwrap());
        } else {
            s.push(BASE64_CHARS.chars().nth(temp3 as usize).unwrap());
            s.push(BASE64_CHARS.chars().nth(temp4 as usize).unwrap());
        }
    }

    s
}

pub fn get_bytes(base64_str: &str) -> Result<Vec<u8>, Box<dyn Error>> {
    let length = base64_str.chars().count();
    if length == 0 {
        return Ok(Vec::new());
    }

    let padding = if base64_str.ends_with(DOUBLE_PAD) {
        2
    } else if base64_str.ends_with(SINGLE_PAD) {
        1
    } else {
        0
    };

    let blocks = (length - 1) / 4 + 1;
    let bytes = blocks * 3;
    let mut data = vec![0u8; bytes - padding];

    for i in 0..blocks {
        let final_block = i == blocks - 1;
        let mut pad2 = false;
        let mut pad1 = false;
        if final_block {
            pad2 = padding == 2;
            pad1 = padding > 0;
        }

        let index = i * 4;
        let temp1 = index_of_base64_chars(base64_str.chars().nth(index).unwrap());
        let temp2 = index_of_base64_chars(base64_str.chars().nth(index + 1).unwrap());
        let temp3 = if pad2 { 0u8 } else { index_of_base64_chars(base64_str.chars().nth(index + 2).unwrap()) };
        let temp4 = if pad1 { 0u8 } else { index_of_base64_chars(base64_str.chars().nth(index + 3).unwrap()) };
        if temp1 == 255 || temp2 == 255 || temp3 == 255 || temp4 == 255 {
            return Err(Box::new(std::io::Error::new(std::io::ErrorKind::Other, "Invalid base64 character.")));
        }

        let b1 = (temp1 << 2) | ((temp2 & 0x30) >> 4);
        let b2 = ((temp2 & 0x0F) << 4) | ((temp3 & 0x3C) >> 2);
        let b3 = ((temp3 & 0x03) << 6) | temp4;

        let index = i * 3;
        data[index] = b1;
        if !pad2 {
            data[index + 1] = b2;
        }
        if !pad1 {
            data[index + 2] = b3;
        }
    }

    Ok(data)
}
