class YinYangBase64:
    __BASE64CHARS__ = "陰陽天干地支五行金木水火土甲乙丙丁戊己庚辛壬癸子丑寅卯辰巳午未申酉戌亥立春驚蟄清明夏芒種小暑秋白露寒冬大雪雨分穀滿至處霜降沖剋歲"
    __SINGLEPAD__ = "宮"
    __DOUBLEPAD__ = "星宿"

    @staticmethod
    def get_str(byte_array: bytes) -> str:
        length = 0 if byte_array is None else len(byte_array)
        if length == 0:
            return ""
        
        padding = length % 3
        if padding > 0:
            padding = 3 - padding
        blocks = (length - 1) // 3 + 1
        
        s = []
        
        for i in range(blocks):
            final_block = i == blocks - 1
            pad2 = pad1 = False
            if final_block:
                pad2 = padding == 2
                pad1 = padding > 0
            
            index = i * 3
            b1 = byte_array[index]
            b2 = 0 if pad2 else byte_array[index + 1]
            b3 = 0 if pad1 else byte_array[index + 2]
            
            temp1 = (b1 & 0xFC) >> 2
            temp2 = ((b1 & 0x03) << 4) + ((b2 & 0xF0) >> 4)
            temp3 = ((b2 & 0x0F) << 2) + ((b3 & 0xC0) >> 6)
            temp4 = b3 & 0x3F
            
            index = i * 4
            s.append(YinYangBase64.__BASE64CHARS__[temp1])
            s.append(YinYangBase64.__BASE64CHARS__[temp2])
            if pad1 and pad2:
                s.append(YinYangBase64.__DOUBLEPAD__[0])
                s.append(YinYangBase64.__DOUBLEPAD__[1])
            elif pad1:
                s.append(YinYangBase64.__BASE64CHARS__[temp3])
                s.append(YinYangBase64.__SINGLEPAD__[0])
            else:
                s.append(YinYangBase64.__BASE64CHARS__[temp3])
                s.append(YinYangBase64.__BASE64CHARS__[temp4])
        
        return "".join(s)

    @staticmethod
    def get_bytes(string: str) -> bytes:
        length = 0 if string is None else len(string)
        if length == 0:
            return bytes()
        
        padding = 2 if string.endswith(YinYangBase64.__DOUBLEPAD__) else 1 if string.endswith(YinYangBase64.__SINGLEPAD__) else 0
        blocks = (length - 1) // 4 + 1
        bytes_length = blocks * 3
        
        data = bytearray(bytes_length - padding)
        
        for i in range(blocks):
            final_block = i == blocks - 1
            pad2 = pad1 = False
            if final_block:
                pad2 = padding == 2
                pad1 = padding > 0
            
            index = i * 4
            temp1 = YinYangBase64.__BASE64CHARS__.index(string[index])
            temp2 = YinYangBase64.__BASE64CHARS__.index(string[index + 1])
            temp3 = YinYangBase64.__BASE64CHARS__.index(string[index + 2])
            temp4 = YinYangBase64.__BASE64CHARS__.index(string[index + 3])
            
            b1 = (temp1 << 2) + ((temp2 & 0x30) >> 4)
            b2 = ((temp2 & 0x0F) << 4) + ((temp3 & 0x3C) >> 2)
            b3 = ((temp3 & 0x03) << 6) + temp4
            
            index = i * 3
            data[index] = b1
            if not pad2:
                data[index + 1] = b2
            if not pad1:
                data[index + 2] = b3
        
        return bytes(data)