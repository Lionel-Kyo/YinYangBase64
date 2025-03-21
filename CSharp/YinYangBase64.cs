/// Reference: http://www.csharp411.com/convert-binary-to-base64-string/
namespace YinYang
{
    public class Base64
    {
        // Removed repeat characters
        // 陰陽天干地支五行金木水火土甲乙丙丁戊己庚辛壬癸子丑寅卯辰巳午未申酉戌亥立春驚蟄清明立夏芒種小暑立秋白露寒露立冬大雪小寒雨水春分穀雨小滿夏至大暑處暑秋分霜降小雪冬至大寒沖剋歲
        private static readonly string Base64Chars = "陰陽天干地支五行金木水火土甲乙丙丁戊己庚辛壬癸子丑寅卯辰巳午未申酉戌亥立春驚蟄清明夏芒種小暑秋白露寒冬大雪雨分穀滿至處霜降沖剋歲";
        private static readonly string SinglePad = "宮";
        private static readonly string DoublePad = "星宿";


        public static string GetString(byte[] bytes)
        {
            int length = bytes == null ? 0 : bytes.Length;
            if (length == 0)
                return "";

            int padding = length % 3;
            if (padding > 0)
                padding = 3 - padding;
            int blocks = (length - 1) / 3 + 1;

            char[] s = new char[blocks * 4];

            for (int i = 0; i < blocks; i++)
            {
                bool finalBlock = i == blocks - 1;
                bool pad2 = false;
                bool pad1 = false;
                if (finalBlock)
                {
                    pad2 = padding == 2;
                    pad1 = padding > 0;
                }

                int index = i * 3;
                byte b1 = bytes[index];
                byte b2 = pad2 ? (byte)0 : bytes[index + 1];
                byte b3 = pad1 ? (byte)0 : bytes[index + 2];

                byte temp1 = (byte)((b1 & 0xFC) >> 2);

                byte temp = (byte)((b1 & 0x03) << 4);
                byte temp2 = (byte)((b2 & 0xF0) >> 4);
                temp2 += temp;

                temp = (byte)((b2 & 0x0F) << 2);
                byte temp3 = (byte)((b3 & 0xC0) >> 6);
                temp3 += temp;

                byte temp4 = (byte)(b3 & 0x3F);

                index = i * 4;
                s[index] = Base64Chars[temp1];
                s[index + 1] = Base64Chars[temp2];
                if (pad1 && pad2)
                {
                    s[index + 2] = DoublePad[0];
                    s[index + 3] = DoublePad[1];
                }
                else if (pad1)
                {
                    s[index + 2] = Base64Chars[temp3];
                    s[index + 3] = SinglePad[0];
                }
                else
                {
                    s[index + 2] = Base64Chars[temp3];
                    s[index + 3] = Base64Chars[temp4];
                }
            }

            return new string(s);
        }

        public static byte[] GetBytes(string str)
        {
            int length = str == null ? 0 : str.Length;
            if (length == 0)
                return new byte[0];

            int padding = str.EndsWith(DoublePad) ? 2 : str.EndsWith(SinglePad) ? 1 : 0;

            int blocks = (length - 1) / 4 + 1;
            int bytes = blocks * 3;

            byte[] data = new byte[bytes - padding];

            for (int i = 0; i < blocks; i++)
            {
                bool finalBlock = i == blocks - 1;
                bool pad2 = false;
                bool pad1 = false;
                if (finalBlock)
                {
                    pad2 = padding == 2;
                    pad1 = padding > 0;
                }

                int index = i * 4;
                byte temp1 = (byte)Base64Chars.IndexOf(str[index]);
                byte temp2 = (byte)Base64Chars.IndexOf(str[index + 1]);
                byte temp3 = pad2 ? (byte)63 : (byte)Base64Chars.IndexOf(str[index + 2]);
                byte temp4 = pad1 ? (byte)63 : (byte)Base64Chars.IndexOf(str[index + 3]);

                byte b1 = (byte)((byte)(temp1 << 2) + (byte)((temp2 & 0x30) >> 4));
                byte b2 = (byte)((byte)((temp2 & 0x0F) << 4) + (byte)((temp3 & 0x3C) >> 2));
                byte b3 = (byte)((byte)((temp3 & 0x03) << 6) + (byte)temp4);

                index = i * 3;
                data[index] = b1;
                if (!pad2)
                    data[index + 1] = b2;
                if (!pad1)
                    data[index + 2] = b3;
            }

            return data;
        }
    }
}