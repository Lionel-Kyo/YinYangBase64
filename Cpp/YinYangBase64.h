#pragma once
#ifdef __cplusplus

#ifndef _YINYANGBASE64_H_
#define _YINYANGBASE64_H_

#include <string>
#include <vector>

namespace YinYang
{
    class Base64
    {
    private:
        static constexpr wchar_t Base64Chars[] = L"陰陽天干地支五行金木水火土甲乙丙丁戊己庚辛壬癸子丑寅卯辰巳午未申酉戌亥立春驚蟄清明夏芒種小暑秋白露寒冬大雪雨分穀滿至處霜降生剋沖";
        static constexpr wchar_t SinglePad[] = L"宮";
        static constexpr wchar_t DoublePad[] = L"星宿";

        static uint8_t FindBase64Chars(wchar_t c) 
        {
            for (uint8_t i = 0; i < 64; i++) 
            {
                if (Base64Chars[i] == c)
                    return i;
            }
            return 255;
        }

    public:
        static std::wstring GetWString(const uint8_t* bytes, size_t length)
        {
            if (length == 0)
                return L"";

            size_t padding = length % 3;
            if (padding > 0) 
                padding = 3 - padding;
            size_t blocks = (length - 1) / 3 + 1;

            std::wstring s;
            s.reserve(blocks * 4);

            for (size_t i = 0; i < blocks; i++)
            {
                bool finalBlock = i == blocks - 1;
                bool pad2 = false;
                bool pad1 = false;
                if (finalBlock)
                {
                    pad2 = padding == 2;
                    pad1 = padding > 0;
                }

                size_t index = i * 3;
                uint8_t b1 = bytes[index];
                uint8_t b2 = pad2 ? 0 : bytes[index + 1];
                uint8_t b3 = pad1 ? 0 : bytes[index + 2];

                uint8_t temp1 = (b1 & 0xFC) >> 2;
                uint8_t temp2 = ((b1 & 0x03) << 4) + ((b2 & 0xF0) >> 4);
                uint8_t temp3 = ((b2 & 0x0F) << 2) + ((b3 & 0xC0) >> 6);
                uint8_t temp4 = b3 & 0x3F;

                s += Base64Chars[temp1];
                s += Base64Chars[temp2];
                if (pad1 && pad2)
                {
                    s += DoublePad[0];
                    s += DoublePad[1];
                }
                else if (pad1)
                {
                    s += Base64Chars[temp3];
                    s += SinglePad[0];
                }
                else
                {
                    s += Base64Chars[temp3];
                    s += Base64Chars[temp4];
                }
            }
            return s;
        }

        static std::wstring GetWString(const std::vector<uint8_t>& bytes)
        {
            return GetWString(bytes.data(), bytes.size());
        }

        static std::vector<uint8_t> GetBytes(const wchar_t* wstr, size_t length)
        {
            if (length == 0)
                return {};

            size_t padding = (wstr[length - 2] == DoublePad[0] && wstr[length - 1] == DoublePad[1]) ? 2 : (wstr[length - 1] == SinglePad[0]) ? 1 : 0;
            size_t blocks = (length - 1) / 4 + 1;
            size_t bytesLength = blocks * 3;

            std::vector<uint8_t> data(bytesLength - padding);

            for (size_t i = 0; i < blocks; i++)
            {
                bool finalBlock = (i == blocks - 1);
                bool pad2 = false;
                bool pad1 = false;
                if (finalBlock)
                {
                    pad2 = (padding == 2);
                    pad1 = (padding > 0);
                }

                size_t index = i * 4;
                uint8_t temp1 = FindBase64Chars(wstr[index]);
                uint8_t temp2 = FindBase64Chars(wstr[index + 1]);
                uint8_t temp3 = pad2 ? (uint8_t)0 : FindBase64Chars(wstr[index + 2]);
                uint8_t temp4 = pad1 ? (uint8_t)0 : FindBase64Chars(wstr[index + 3]);

                uint8_t b1 = (uint8_t)(temp1 << 2) + (uint8_t)((temp2 & 0x30) >> 4);
                uint8_t b2 = (uint8_t)((temp2 & 0x0F) << 4) + (uint8_t)((temp3 & 0x3C) >> 2);
                uint8_t b3 = (uint8_t)((temp3 & 0x03) << 6) + (uint8_t)temp4;

                index = i * 3;
                data[index] = b1;
                if (!pad2)
                    data[index + 1] = b2;
                if (!pad1)
                    data[index + 2] = b3;
            }
            return data;
        }

        static std::vector<uint8_t> GetBytes(const std::wstring& wstr)
        {
            return GetBytes(wstr.c_str(), wstr.size());
        }
    };
}

#endif // !_YINYANGBASE64_H_
#endif // __cplusplus