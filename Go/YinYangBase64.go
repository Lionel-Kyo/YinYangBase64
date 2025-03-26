package YinYangBase64

import (
	"errors"
	"strings"
)

var base64Chars = []rune("陰陽天干地支五行金木水火土甲乙丙丁戊己庚辛壬癸子丑寅卯辰巳午未申酉戌亥立春驚蟄清明夏芒種小暑秋白露寒冬大雪雨分穀滿至處霜降生剋沖")
var singlePad = []rune("宮")
var doublePad = []rune("星宿")

func indexOfBase64Chars(r rune) byte {
	for _i, _r := range base64Chars {
		if r == _r {
			return byte(_i)
		}
	}
	return 255
}

func GetString(bytes []byte) string {
	length := len(bytes)
	if length == 0 {
		return ""
	}

	padding := length % 3
	if padding > 0 {
		padding = 3 - padding
	}
	blocks := (length-1)/3 + 1

	var s strings.Builder

	for i := 0; i < blocks; i++ {
		finalBlock := i == blocks-1
		pad2 := false
		pad1 := false
		if finalBlock {
			pad2 = padding == 2
			pad1 = padding > 0
		}

		index := i * 3
		b1 := bytes[index]
		b2 := byte(0)
		if !pad2 {
			b2 = bytes[index+1]
		}
		b3 := byte(0)
		if !pad1 {
			b3 = bytes[index+2]
		}

		temp1 := (b1 & 0xFC) >> 2
		temp2 := ((b1 & 0x03) << 4) | ((b2 & 0xF0) >> 4)
		temp3 := ((b2 & 0x0F) << 2) | ((b3 & 0xC0) >> 6)
		temp4 := b3 & 0x3F

		s.WriteRune(base64Chars[temp1])
		s.WriteRune(base64Chars[temp2])
		if pad1 && pad2 {
			s.WriteRune(doublePad[0])
			s.WriteRune(doublePad[1])
		} else if pad1 {
			s.WriteRune(base64Chars[temp3])
			s.WriteRune(singlePad[0])
		} else {
			s.WriteRune(base64Chars[temp3])
			s.WriteRune(base64Chars[temp4])
		}
	}

	return s.String()
}

func GetBytes(str string) ([]byte, error) {
	runes := []rune(str)
	length := len(runes)
	if length == 0 {
		return []byte{}, nil
	}

	padding := 0
	if runes[length-2] == doublePad[0] && runes[length-1] == doublePad[1] {
		padding = 2
	} else if runes[length-1] == singlePad[0] {
		padding = 1
	}

	blocks := (length-1)/4 + 1
	bytes := blocks * 3
	data := make([]byte, bytes-padding)

	for i := 0; i < blocks; i++ {
		finalBlock := i == blocks-1
		pad2 := false
		pad1 := false
		if finalBlock {
			pad2 = padding == 2
			pad1 = padding > 0
		}

		index := i * 4
		temp1 := indexOfBase64Chars(runes[index])
		temp2 := indexOfBase64Chars(runes[index+1])
		temp3 := byte(0)
		if !pad2 {
			temp3 = indexOfBase64Chars(runes[index+2])
		}
		temp4 := byte(0)
		if !pad1 {
			temp4 = indexOfBase64Chars(runes[index+3])
		}

		if temp1 == 255 || temp2 == 255 || temp3 == 255 || temp4 == 255 {
			return nil, errors.New("invalid base64 character")
		}

		b1 := byte(temp1<<2) | byte((temp2&0x30)>>4)
		b2 := byte((temp2&0x0F)<<4) | byte((temp3&0x3C)>>2)
		b3 := byte((temp3&0x03)<<6) | byte(temp4)

		index = i * 3
		data[index] = b1
		if !pad2 {
			data[index+1] = b2
		}
		if !pad1 {
			data[index+2] = b3
		}
	}

	return data, nil
}
