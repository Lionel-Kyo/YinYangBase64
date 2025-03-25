const Base64Chars: string = "陰陽天干地支五行金木水火土甲乙丙丁戊己庚辛壬癸子丑寅卯辰巳午未申酉戌亥立春驚蟄清明夏芒種小暑秋白露寒冬大雪雨分穀滿至處霜降生剋沖";
const SinglePad: string = "宮";
const DoublePad: string = "星宿";

const getString = (bytes: Uint8Array): string => {
    let length = bytes ? bytes.length : 0;
    if (length === 0) 
        return "";

    let padding = length % 3;
    if (padding > 0) 
        padding = 3 - padding;
    let blocks = Math.floor((length - 1) / 3) + 1;

    let s: string[] = new Array(blocks * 4);

    for (let i = 0; i < blocks; i++) {
        let finalBlock = i === blocks - 1;
        let pad2 = false;
        let pad1 = false;
        if (finalBlock) {
            pad2 = padding === 2;
            pad1 = padding > 0;
        }

        let index = i * 3;
        let b1 = bytes[index];
        let b2 = pad2 ? 0 : bytes[index + 1];
        let b3 = pad1 ? 0 : bytes[index + 2];

        let temp1 = (b1 & 0xFC) >> 2;
        let temp2 = ((b1 & 0x03) << 4) + ((b2 & 0xF0) >> 4);
        let temp3 = ((b2 & 0x0F) << 2) + ((b3 & 0xC0) >> 6);
        let temp4 = b3 & 0x3F;

        index = i * 4;
        s[index] = Base64Chars[temp1];
        s[index + 1] = Base64Chars[temp2];
        if (pad1 && pad2) {
            s[index + 2] = DoublePad[0];
            s[index + 3] = DoublePad[1];
        } else if (pad1) {
            s[index + 2] = Base64Chars[temp3];
            s[index + 3] = SinglePad[0];
        } else {
            s[index + 2] = Base64Chars[temp3];
            s[index + 3] = Base64Chars[temp4];
        }
    }
    return s.join("");
}

const getBytes = (str: string): Uint8Array => {
    let length = str ? str.length : 0;
    if (length === 0) 
        return new Uint8Array(0);

    let padding = str.endsWith(DoublePad) ? 2 : str.endsWith(SinglePad) ? 1 : 0;
    let blocks = Math.floor((length - 1) / 4) + 1;
    let bytes = blocks * 3;

    let data = new Uint8Array(bytes - padding);

    for (let i = 0; i < blocks; i++) {
        let finalBlock = i === blocks - 1;
        let pad2 = false;
        let pad1 = false;
        if (finalBlock) {
            pad2 = padding === 2;
            pad1 = padding > 0;
        }

        let index = i * 4;
        let temp1 = Base64Chars.indexOf(str[index]);
        let temp2 = Base64Chars.indexOf(str[index + 1]);
        let temp3 = pad2 ? 0 : Base64Chars.indexOf(str[index + 2]);
        let temp4 = pad1 ? 0 : Base64Chars.indexOf(str[index + 3]);
        if (temp1 == -1 || temp2 == -1 || temp3 == -1 || temp4 == -1) 
            throw new Error("Invalid base64 character.");

        let b1 = (temp1 << 2) + ((temp2 & 0x30) >> 4);
        let b2 = ((temp2 & 0x0F) << 4) + ((temp3 & 0x3C) >> 2);
        let b3 = ((temp3 & 0x03) << 6) + temp4;

        index = i * 3;
        data[index] = b1;
        if (!pad2) 
            data[index + 1] = b2;
        if (!pad1) 
            data[index + 2] = b3;
    }
    return data;
}

const YinYangBase64 = { getString, getBytes };

export default YinYangBase64;