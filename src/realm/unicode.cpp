/*************************************************************************
 *
 * Copyright 2016 Realm Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 **************************************************************************/

#include <algorithm>
#include <vector>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#else
#include <ctype.h>
#endif

#include <realm/util/safe_int_ops.hpp>
#include <realm/unicode.hpp>

#include <clocale>

#ifdef _MSC_VER
#include <codecvt>
#else
#include <locale>
#endif


using namespace realm;

namespace {

std::wstring utf8_to_wstring(StringData str)
{
#if defined(_MSC_VER)
    // __STDC_UTF_16__ seems not to work
    static_assert(sizeof(wchar_t) == 2, "Expected Windows to use utf16");
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> utf8conv;
    auto w_result = utf8conv.from_bytes(str.data(), str.data() + str.size());
    return w_result;
#else
    // gcc 4.7 and 4.8 do not yet support codecvt_utf8_utf16 and wstring_convert, and note that we can NOT just use
    // setlocale() + mbstowcs() because setlocale is extremely slow and may change locale of the entire user process
    static_cast<void>(str);
    REALM_ASSERT(false);
    return L"";
#endif
}

} // unnamed namespace


namespace realm {

constexpr size_t last_latin_extended_2_unicode = 591;

// Converts unicodes 0...0x6ff (up to Arabic) to their respective lower case characters using a popular
// UnicodeData.txtfile (http://www.opensource.apple.com/source/Heimdal/Heimdal-247.9/lib/wind/UnicodeData.txt) that
// contains case conversion information. The conversion does not take your current locale in count; it can be
// slightly wrong in some countries! If the input is already lower case, or outside range 0...0x6ff, then input value
// is returned untouched.
unsigned int unicode_case_convert(unsigned int unicode, bool upper)
{
    constexpr uint32_t last_unicode = 1023; // last greek unicode
    // clang-format off
    static const uint32_t upper_lower[last_unicode + 1][2] = { { 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0x0061 },{ 0, 0x0062 },{ 0, 0x0063 },{ 0, 0x0064 },{ 0, 0x0065 },{ 0, 0x0066 },{ 0, 0x0067 },{ 0, 0x0068 },{ 0, 0x0069 },{ 0, 0x006A },{ 0, 0x006B },{ 0, 0x006C },{ 0, 0x006D },{ 0, 0x006E },{ 0, 0x006F },{ 0, 0x0070 },{ 0, 0x0071 },{ 0, 0x0072 },{ 0, 0x0073 },{ 0, 0x0074 },{ 0, 0x0075 },{ 0, 0x0076 },{ 0, 0x0077 },{ 0, 0x0078 },{ 0, 0x0079 },{ 0, 0x007A },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0x0041, 0 },{ 0x0042, 0 },{ 0x0043, 0 },{ 0x0044, 0 },{ 0x0045, 0 },{ 0x0046, 0 },{ 0x0047, 0 },{ 0x0048, 0 },{ 0x0049, 0 },{ 0x004A, 0 },{ 0x004B, 0 },{ 0x004C, 0 },{ 0x004D, 0 },{ 0x004E, 0 },{ 0x004F, 0 },{ 0x0050, 0 },{ 0x0051, 0 },{ 0x0052, 0 },{ 0x0053, 0 },{ 0x0054, 0 },{ 0x0055, 0 },{ 0x0056, 0 },{ 0x0057, 0 },{ 0x0058, 0 },{ 0x0059, 0 },{ 0x005A, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0x039C, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0x00E0 },{ 0, 0x00E1 },{ 0, 0x00E2 },{ 0, 0x00E3 },{ 0, 0x00E4 },{ 0, 0x00E5 },{ 0, 0x00E6 },{ 0, 0x00E7 },{ 0, 0x00E8 },{ 0, 0x00E9 },{ 0, 0x00EA },{ 0, 0x00EB },{ 0, 0x00EC },{ 0, 0x00ED },{ 0, 0x00EE },{ 0, 0x00EF },{ 0, 0x00F0 },{ 0, 0x00F1 },{ 0, 0x00F2 },{ 0, 0x00F3 },{ 0, 0x00F4 },{ 0, 0x00F5 },{ 0, 0x00F6 },{ 0, 0 },{ 0, 0x00F8 },{ 0, 0x00F9 },{ 0, 0x00FA },{ 0, 0x00FB },{ 0, 0x00FC },{ 0, 0x00FD },{ 0, 0x00FE },{ 0, 0 },{ 0x00C0, 0 },{ 0x00C1, 0 },{ 0x00C2, 0 },{ 0x00C3, 0 },{ 0x00C4, 0 },{ 0x00C5, 0 },{ 0x00C6, 0 },{ 0x00C7, 0 },{ 0x00C8, 0 },{ 0x00C9, 0 },{ 0x00CA, 0 },{ 0x00CB, 0 },{ 0x00CC, 0 },{ 0x00CD, 0 },{ 0x00CE, 0 },{ 0x00CF, 0 },{ 0x00D0, 0 },{ 0x00D1, 0 },{ 0x00D2, 0 },{ 0x00D3, 0 },{ 0x00D4, 0 },{ 0x00D5, 0 },{ 0x00D6, 0 },{ 0, 0 },{ 0x00D8, 0 },{ 0x00D9, 0 },{ 0x00DA, 0 },{ 0x00DB, 0 },{ 0x00DC, 0 },{ 0x00DD, 0 },{ 0x00DE, 0 },{ 0x0178, 0 },{ 0, 0x0101 },{ 0x0100, 0 },{ 0, 0x0103 },{ 0x0102, 0 },{ 0, 0x0105 },{ 0x0104, 0 },{ 0, 0x0107 },{ 0x0106, 0 },{ 0, 0x0109 },{ 0x0108, 0 },{ 0, 0x010B },{ 0x010A, 0 },{ 0, 0x010D },{ 0x010C, 0 },{ 0, 0x010F },{ 0x010E, 0 },{ 0, 0x0111 },{ 0x0110, 0 },{ 0, 0x0113 },{ 0x0112, 0 },{ 0, 0x0115 },{ 0x0114, 0 },{ 0, 0x0117 },{ 0x0116, 0 },{ 0, 0x0119 },{ 0x0118, 0 },{ 0, 0x011B },{ 0x011A, 0 },{ 0, 0x011D },{ 0x011C, 0 },{ 0, 0x011F },{ 0x011E, 0 },{ 0, 0x0121 },{ 0x0120, 0 },{ 0, 0x0123 },{ 0x0122, 0 },{ 0, 0x0125 },{ 0x0124, 0 },{ 0, 0x0127 },{ 0x0126, 0 },{ 0, 0x0129 },{ 0x0128, 0 },{ 0, 0x012B },{ 0x012A, 0 },{ 0, 0x012D },{ 0x012C, 0 },{ 0, 0x012F },{ 0x012E, 0 },{ 0, 0x0069 },{ 0x0049, 0 },{ 0, 0x0133 },{ 0x0132, 0 },{ 0, 0x0135 },{ 0x0134, 0 },{ 0, 0x0137 },{ 0x0136, 0 },{ 0, 0 },{ 0, 0x013A },{ 0x0139, 0 },{ 0, 0x013C },{ 0x013B, 0 },{ 0, 0x013E },{ 0x013D, 0 },{ 0, 0x0140 },{ 0x013F, 0 },{ 0, 0x0142 },{ 0x0141, 0 },{ 0, 0x0144 },{ 0x0143, 0 },{ 0, 0x0146 },{ 0x0145, 0 },{ 0, 0x0148 },{ 0x0147, 0 },{ 0, 0 },{ 0, 0x014B },{ 0x014A, 0 },{ 0, 0x014D },{ 0x014C, 0 },{ 0, 0x014F },{ 0x014E, 0 },{ 0, 0x0151 },{ 0x0150, 0 },{ 0, 0x0153 },{ 0x0152, 0 },{ 0, 0x0155 },{ 0x0154, 0 },{ 0, 0x0157 },{ 0x0156, 0 },{ 0, 0x0159 },{ 0x0158, 0 },{ 0, 0x015B },{ 0x015A, 0 },{ 0, 0x015D },{ 0x015C, 0 },{ 0, 0x015F },{ 0x015E, 0 },{ 0, 0x0161 },{ 0x0160, 0 },{ 0, 0x0163 },{ 0x0162, 0 },{ 0, 0x0165 },{ 0x0164, 0 },{ 0, 0x0167 },{ 0x0166, 0 },{ 0, 0x0169 },{ 0x0168, 0 },{ 0, 0x016B },{ 0x016A, 0 },{ 0, 0x016D },{ 0x016C, 0 },{ 0, 0x016F },{ 0x016E, 0 },{ 0, 0x0171 },{ 0x0170, 0 },{ 0, 0x0173 },{ 0x0172, 0 },{ 0, 0x0175 },{ 0x0174, 0 },{ 0, 0x0177 },{ 0x0176, 0 },{ 0, 0x00FF },{ 0, 0x017A },{ 0x0179, 0 },{ 0, 0x017C },{ 0x017B, 0 },{ 0, 0x017E },{ 0x017D, 0 },{ 0x0053, 0 },{ 0, 0 },{ 0, 0x0253 },{ 0, 0x0183 },{ 0x0182, 0 },{ 0, 0x0185 },{ 0x0184, 0 },{ 0, 0x0254 },{ 0, 0x0188 },{ 0x0187, 0 },{ 0, 0x0256 },{ 0, 0x0257 },{ 0, 0x018C },{ 0x018B, 0 },{ 0, 0 },{ 0, 0x01DD },{ 0, 0x0259 },{ 0, 0x025B },{ 0, 0x0192 },{ 0x0191, 0 },{ 0, 0x0260 },{ 0, 0x0263 },{ 0x01F6, 0 },{ 0, 0x0269 },{ 0, 0x0268 },{ 0, 0x0199 },{ 0x0198, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0x026F },{ 0, 0x0272 },{ 0x0220, 0 },{ 0, 0x0275 },{ 0, 0x01A1 },{ 0x01A0, 0 },{ 0, 0x01A3 },{ 0x01A2, 0 },{ 0, 0x01A5 },{ 0x01A4, 0 },{ 0, 0x0280 },{ 0, 0x01A8 },{ 0x01A7, 0 },{ 0, 0x0283 },{ 0, 0 },{ 0, 0 },{ 0, 0x01AD },{ 0x01AC, 0 },{ 0, 0x0288 },{ 0, 0x01B0 },{ 0x01AF, 0 },{ 0, 0x028A },{ 0, 0x028B },{ 0, 0x01B4 },{ 0x01B3, 0 },{ 0, 0x01B6 },{ 0x01B5, 0 },{ 0, 0x0292 },{ 0, 0x01B9 },{ 0x01B8, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0x01BD },{ 0x01BC, 0 },{ 0, 0 },{ 0x01F7, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0x01C6 },{ 0x01C4, 0x01C6 },{ 0x01C4, 0 },{ 0, 0x01C9 },{ 0x01C7, 0x01C9 },{ 0x01C7, 0 },{ 0, 0x01CC },{ 0x01CA, 0x01CC },{ 0x01CA, 0 },{ 0, 0x01CE },{ 0x01CD, 0 },{ 0, 0x01D0 },{ 0x01CF, 0 },{ 0, 0x01D2 },{ 0x01D1, 0 },{ 0, 0x01D4 },{ 0x01D3, 0 },{ 0, 0x01D6 },{ 0x01D5, 0 },{ 0, 0x01D8 },{ 0x01D7, 0 },{ 0, 0x01DA },{ 0x01D9, 0 },{ 0, 0x01DC },{ 0x01DB, 0 },{ 0x018E, 0 },{ 0, 0x01DF },{ 0x01DE, 0 },{ 0, 0x01E1 },{ 0x01E0, 0 },{ 0, 0x01E3 },{ 0x01E2, 0 },{ 0, 0x01E5 },{ 0x01E4, 0 },{ 0, 0x01E7 },{ 0x01E6, 0 },{ 0, 0x01E9 },{ 0x01E8, 0 },{ 0, 0x01EB },{ 0x01EA, 0 },{ 0, 0x01ED },{ 0x01EC, 0 },{ 0, 0x01EF },{ 0x01EE, 0 },{ 0, 0 },{ 0, 0x01F3 },{ 0x01F1, 0x01F3 },{ 0x01F1, 0 },{ 0, 0x01F5 },{ 0x01F4, 0 },{ 0, 0x0195 },{ 0, 0x01BF },{ 0, 0x01F9 },{ 0x01F8, 0 },{ 0, 0x01FB },{ 0x01FA, 0 },{ 0, 0x01FD },{ 0x01FC, 0 },{ 0, 0x01FF },{ 0x01FE, 0 },{ 0, 0x0201 },{ 0x0200, 0 },{ 0, 0x0203 },{ 0x0202, 0 },{ 0, 0x0205 },{ 0x0204, 0 },{ 0, 0x0207 },{ 0x0206, 0 },{ 0, 0x0209 },{ 0x0208, 0 },{ 0, 0x020B },{ 0x020A, 0 },{ 0, 0x020D },{ 0x020C, 0 },{ 0, 0x020F },{ 0x020E, 0 },{ 0, 0x0211 },{ 0x0210, 0 },{ 0, 0x0213 },{ 0x0212, 0 },{ 0, 0x0215 },{ 0x0214, 0 },{ 0, 0x0217 },{ 0x0216, 0 },{ 0, 0x0219 },{ 0x0218, 0 },{ 0, 0x021B },{ 0x021A, 0 },{ 0, 0x021D },{ 0x021C, 0 },{ 0, 0x021F },{ 0x021E, 0 },{ 0, 0x019E },{ 0, 0 },{ 0, 0x0223 },{ 0x0222, 0 },{ 0, 0x0225 },{ 0x0224, 0 },{ 0, 0x0227 },{ 0x0226, 0 },{ 0, 0x0229 },{ 0x0228, 0 },{ 0, 0x022B },{ 0x022A, 0 },{ 0, 0x022D },{ 0x022C, 0 },{ 0, 0x022F },{ 0x022E, 0 },{ 0, 0x0231 },{ 0x0230, 0 },{ 0, 0x0233 },{ 0x0232, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0x0181, 0 },{ 0x0186, 0 },{ 0, 0 },{ 0x0189, 0 },{ 0x018A, 0 },{ 0, 0 },{ 0x018F, 0 },{ 0, 0 },{ 0x0190, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0x0193, 0 },{ 0, 0 },{ 0, 0 },{ 0x0194, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0x0197, 0 },{ 0x0196, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0x019C, 0 },{ 0, 0 },{ 0, 0 },{ 0x019D, 0 },{ 0, 0 },{ 0, 0 },{ 0x019F, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0x01A6, 0 },{ 0, 0 },{ 0, 0 },{ 0x01A9, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0x01AE, 0 },{ 0, 0 },{ 0x01B1, 0 },{ 0x01B2, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0x01B7, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0x0399, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0x03AC },{ 0, 0 },{ 0, 0x03AD },{ 0, 0x03AE },{ 0, 0x03AF },{ 0, 0 },{ 0, 0x03CC },{ 0, 0 },{ 0, 0x03CD },{ 0, 0x03CE },{ 0, 0 },{ 0, 0x03B1 },{ 0, 0x03B2 },{ 0, 0x03B3 },{ 0, 0x03B4 },{ 0, 0x03B5 },{ 0, 0x03B6 },{ 0, 0x03B7 },{ 0, 0x03B8 },{ 0, 0x03B9 },{ 0, 0x03BA },{ 0, 0x03BB },{ 0, 0x03BC },{ 0, 0x03BD },{ 0, 0x03BE },{ 0, 0x03BF },{ 0, 0x03C0 },{ 0, 0x03C1 },{ 0, 0 },{ 0, 0x03C3 },{ 0, 0x03C4 },{ 0, 0x03C5 },{ 0, 0x03C6 },{ 0, 0x03C7 },{ 0, 0x03C8 },{ 0, 0x03C9 },{ 0, 0x03CA },{ 0, 0x03CB },{ 0x0386, 0 },{ 0x0388, 0 },{ 0x0389, 0 },{ 0x038A, 0 },{ 0, 0 },{ 0x0391, 0 },{ 0x0392, 0 },{ 0x0393, 0 },{ 0x0394, 0 },{ 0x0395, 0 },{ 0x0396, 0 },{ 0x0397, 0 },{ 0x0398, 0 },{ 0x0399, 0 },{ 0x039A, 0 },{ 0x039B, 0 },{ 0x039C, 0 },{ 0x039D, 0 },{ 0x039E, 0 },{ 0x039F, 0 },{ 0x03A0, 0 },{ 0x03A1, 0 },{ 0x03A3, 0 },{ 0x03A3, 0 },{ 0x03A4, 0 },{ 0x03A5, 0 },{ 0x03A6, 0 },{ 0x03A7, 0 },{ 0x03A8, 0 },{ 0x03A9, 0 },{ 0x03AA, 0 },{ 0x03AB, 0 },{ 0x038C, 0 },{ 0x038E, 0 },{ 0x038F, 0 },{ 0, 0 },{ 0x0392, 0 },{ 0x0398, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0x03A6, 0 },{ 0x03A0, 0 },{ 0, 0 },{ 0, 0x03D9 },{ 0x03D8, 0 },{ 0, 0x03DB },{ 0x03DA, 0 },{ 0, 0x03DD },{ 0x03DC, 0 },{ 0, 0x03DF },{ 0x03DE, 0 },{ 0, 0x03E1 },{ 0x03E0, 0 },{ 0, 0x03E3 },{ 0x03E2, 0 },{ 0, 0x03E5 },{ 0x03E4, 0 },{ 0, 0x03E7 },{ 0x03E6, 0 },{ 0, 0x03E9 },{ 0x03E8, 0 },{ 0, 0x03EB },{ 0x03EA, 0 },{ 0, 0x03ED },{ 0x03EC, 0 },{ 0, 0x03EF },{ 0x03EE, 0 },{ 0x039A, 0 },{ 0x03A1, 0 },{ 0x03F9, 0 },{ 0, 0 },{ 0, 0x03B8 },{ 0x0395, 0 },{ 0, 0 },{ 0, 0x03F8 },{ 0x03F7, 0 },{ 0, 0x03F2 },{ 0, 0x03FB },{ 0x03FA, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 },{ 0, 0 } };
     clang-format on

    unsigned int ret;

    if (unicode > last_unicode) {
        ret = unicode;
    }
    else {
        unsigned int converted = upper_lower[unicode][upper ? 0 : 1];
        ret = converted == 0 ? unicode : converted;
    }

    return ret;
}


bool set_string_compare_method(string_compare_method_t method, StringCompareCallback callback)
{
    if (method == STRING_COMPARE_CPP11) {
#if !REALM_ANDROID
        std::string l = std::locale("").name();
        // We cannot use C locale because it puts 'Z' before 'a'
        if (l == "C")
            return false;
#else
        // If Realm wasn't compiled as C++11, just return false.
        return false;
#endif
    }
    else if (method == STRING_COMPARE_CALLBACK) {
        string_compare_callback = callback;
    }

    // other success actions
    string_compare_method = method;
    return true;
}


// clang-format off
// Returns the number of bytes in a UTF-8 sequence whose leading byte is as specified.
size_t sequence_length(char lead)
{
    // keep 'static' else entire array will be pushed to stack at each call
    const static unsigned char lengths[256] = {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 1, 1
    };

    return lengths[static_cast<unsigned char>(lead)];
}
// clang-format on

// Check if the next UTF-8 sequence in [begin, end) is identical to
// the one beginning at begin2. If it is, 'begin' is advanced
// accordingly.
inline bool equal_sequence(const char*& begin, const char* end, const char* begin2)
{
    if (begin[0] != begin2[0])
        return false;

    size_t i = 1;
    if (static_cast<int>(std::char_traits<char>::to_int_type(begin[0])) & 0x80) {
        // All following bytes matching '10xxxxxx' will be considered
        // as part of this character.
        while (begin + i != end) {
            if ((static_cast<int>(std::char_traits<char>::to_int_type(begin[i])) & (0x80 + 0x40)) != 0x80)
                break;
            if (begin[i] != begin2[i])
                return false;
            ++i;
        }
    }

    begin += i;
    return true;
}

// Translate from utf8 char to unicode. No check for invalid utf8; may read out of bounds! Caller must check.
uint32_t utf8value(const char* character)
{
    const unsigned char* c = reinterpret_cast<const unsigned char*>(character);
    size_t len = sequence_length(c[0]);
    uint32_t res = c[0];

    if (len == 1)
        return res;

    res &= (0x3f >> (len - 1));

    for (size_t i = 1; i < len; i++)
        res = ((res << 6) | (c[i] & 0x3f));

    return res;
}

// Returns bool(string1 < string2) for utf-8
bool utf8_compare(StringData string1, StringData string2)
{
    const char* s1 = string1.data();
    const char* s2 = string2.data();

    // This collation_order array has 592 entries; one entry per unicode character in the range 0...591
    // (upto and including 'Latin Extended 2'). The value tells what 'sorting order rank' the character
    // has, such that unichar1 < unichar2 implies collation_order[unichar1] < collation_order[unichar2]. The
    // array is generated from the table found at ftp://ftp.unicode.org/Public/UCA/latest/allkeys.txt. At the
    // bottom of unicode.cpp you can find source code that reads such a file and translates it into C++ that
    // you can copy/paste in case the official table should get updated.
    //
    // NOTE: Some numbers in the array are vere large. This is because the value is the *global* rank of the
    // almost full unicode set. An optimization could be to 'normalize' all values so they ranged from
    // 0...591 so they would fit in a uint16_t array instead of uint32_t.
    //
    // It groups all characters that look visually identical, that is, it puts `a, ‡, Â` together and before
    // `¯, o, ˆ`. Note that this sorting method is wrong in some countries, such as Denmark where `Â` must
    // come last. NOTE: This is a limitation of STRING_COMPARE_CORE until we get better such 'locale' support.

    // clang-format off
    static const uint32_t collation_order_core_similar[last_latin_extended_2_unicode + 1] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 456, 457, 458, 459, 460, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 461, 462, 463, 464, 8130, 465, 466, 467,
        468, 469, 470, 471, 472, 473, 474, 475, 8178, 8248, 8433, 8569, 8690, 8805, 8912, 9002, 9093, 9182, 476, 477, 478, 479, 480, 481, 482, 9290, 9446, 9511, 9595, 9690, 9818, 9882, 9965, 10051, 10156, 10211, 10342, 10408, 10492, 10588,
        10752, 10828, 10876, 10982, 11080, 11164, 11304, 11374, 11436, 11493, 11561, 483, 484, 485, 486, 487, 488, 9272, 9428, 9492, 9575, 9671, 9800, 9864, 9947, 10030, 10138, 10193, 10339, 10389, 10474, 10570, 10734, 10811, 10857, 10964, 11062, 11146, 11285, 11356,
        11417, 11476, 11543, 489, 490, 491, 492, 27, 28, 29, 30, 31, 32, 493, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58,
        494, 495, 8128, 8133, 8127, 8135, 496, 497, 498, 499, 9308, 500, 501, 59, 502, 503, 504, 505, 8533, 8669, 506, 12018, 507, 508, 509, 8351, 10606, 510, 8392, 8377, 8679, 511, 9317, 9315, 9329, 9353, 9348, 9341, 9383, 9545,
        9716, 9714, 9720, 9732, 10078, 10076, 10082, 10086, 9635, 10522, 10615, 10613, 10619, 10640, 10633, 512, 10652, 11190, 11188, 11194, 11202, 11515, 11624, 11038, 9316, 9314, 9328, 9352, 9345, 9340, 9381, 9543, 9715, 9713, 9719, 9731, 10077, 10075, 10081, 10085,
        9633, 10521, 10614, 10612, 10618, 10639, 10630, 513, 10651, 11189, 11187, 11193, 11199, 11514, 11623, 11521, 9361, 9360, 9319, 9318, 9359, 9358, 9536, 9535, 9538, 9537, 9542, 9541, 9540, 9539, 9620, 9619, 9626, 9625, 9744, 9743, 9718, 9717, 9736, 9735,
        9742, 9741, 9730, 9729, 9909, 9908, 9907, 9906, 9913, 9912, 9915, 9914, 9989, 9988, 10000, 9998, 10090, 10089, 10095, 10094, 10080, 10079, 10093, 10092, 10091, 10120, 10113, 10112, 10180, 10179, 10240, 10239, 10856, 10322, 10321, 10326, 10325, 10324, 10323, 10340,
        10337, 10328, 10327, 10516, 10515, 10526, 10525, 10520, 10519, 11663, 10567, 10566, 10660, 10659, 10617, 10616, 10638, 10637, 10689, 10688, 10901, 10900, 10907, 10906, 10903, 10902, 11006, 11005, 11010, 11009, 11018, 11017, 11012, 11011, 11109, 11108, 11104, 11103, 11132, 11131,
        11215, 11214, 11221, 11220, 11192, 11191, 11198, 11197, 11213, 11212, 11219, 11218, 11401, 11400, 11519, 11518, 11522, 11583, 11582, 11589, 11588, 11587, 11586, 11027, 9477, 9486, 9488, 9487, 11657, 11656, 10708, 9568, 9567, 9662, 9664, 9667, 9666, 11594, 9774, 9779,
        9784, 9860, 9859, 9937, 9943, 10014, 10135, 10129, 10266, 10265, 10363, 10387, 11275, 10554, 10556, 10723, 10673, 10672, 9946, 9945, 10802, 10801, 10929, 11653, 11652, 11054, 11058, 11136, 11139, 11138, 11141, 11232, 11231, 11282, 11347, 11537, 11536, 11597, 11596, 11613,
        11619, 11618, 11621, 11645, 11655, 11654, 11125, 11629, 11683, 11684, 11685, 11686, 9654, 9653, 9652, 10345, 10344, 10343, 10541, 10540, 10539, 9339, 9338, 10084, 10083, 10629, 10628, 11196, 11195, 11211, 11210, 11205, 11204, 11209, 11208, 11207, 11206, 9773, 9351, 9350,
        9357, 9356, 9388, 9387, 9934, 9933, 9911, 9910, 10238, 10237, 10656, 10655, 10658, 10657, 11616, 11615, 10181, 9651, 9650, 9648, 9905, 9904, 10015, 11630, 10518, 10517, 9344, 9343, 9386, 9385, 10654, 10653, 9365, 9364, 9367, 9366, 9752, 9751, 9754, 9753,
        10099, 10098, 10101, 10100, 10669, 10668, 10671, 10670, 10911, 10910, 10913, 10912, 11228, 11227, 11230, 11229, 11026, 11025, 11113, 11112, 11542, 11541, 9991, 9990, 10557, 9668, 10731, 10730, 11601, 11600, 9355, 9354, 9738, 9737, 10636, 10635, 10646, 10645, 10648, 10647,
        10650, 10649, 11528, 11527, 10382, 10563, 11142, 10182, 9641, 10848, 9409, 9563, 9562, 10364, 11134, 11048, 11606, 11660, 11659, 9478, 11262, 11354, 9769, 9768, 10186, 10185, 10855, 10854, 10936, 10935, 11535, 11534
    };

    static const uint32_t collation_order_core[last_latin_extended_2_unicode + 1] = {
        0, 2, 3, 4, 5, 6, 7, 8, 9, 33, 34, 35, 36, 37, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 31, 38, 39, 40, 41, 42, 43, 29, 44, 45, 46, 76, 47, 30, 48, 49, 128, 132, 134, 137, 139, 140, 143, 144, 145, 146, 50, 51, 77, 78, 79, 52, 53, 148, 182, 191, 208, 229, 263, 267, 285, 295, 325, 333, 341, 360, 363, 385, 429, 433, 439, 454, 473, 491, 527, 531, 537, 539, 557, 54, 55, 56, 57, 58, 59, 147, 181, 190, 207,
        228, 262, 266, 284, 294, 324, 332, 340, 359, 362, 384, 428, 432, 438, 453, 472, 490, 526, 530, 536, 538, 556, 60, 61, 62, 63, 28, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 32, 64, 72, 73, 74, 75, 65, 88, 66, 89, 149, 81, 90, 1, 91, 67, 92, 80, 136, 138, 68, 93, 94, 95, 69, 133, 386, 82, 129, 130, 131, 70, 153, 151, 157, 165, 575, 588, 570, 201, 233,
        231, 237, 239, 300, 298, 303, 305, 217, 371, 390, 388, 394, 402, 584, 83, 582, 495, 493, 497, 555, 541, 487, 470, 152, 150, 156, 164, 574, 587, 569, 200, 232, 230, 236, 238, 299, 297, 302, 304, 216, 370, 389, 387, 393, 401, 583, 84, 581, 494, 492, 496, 554, 540, 486, 544, 163, 162, 161, 160, 167, 166, 193, 192, 197, 196, 195, 194, 199, 198, 210, 209, 212, 211, 245, 244, 243, 242, 235, 234, 247, 246, 241, 240, 273, 272, 277, 276, 271, 270, 279, 278, 287, 286, 291, 290, 313, 312, 311, 310, 309,
        308, 315, 314, 301, 296, 323, 322, 328, 327, 337, 336, 434, 343, 342, 349, 348, 347, 346, 345, 344, 353, 352, 365, 364, 373, 372, 369, 368, 375, 383, 382, 400, 399, 398, 397, 586, 585, 425, 424, 442, 441, 446, 445, 444, 443, 456, 455, 458, 457, 462, 461, 460, 459, 477, 476, 475, 474, 489, 488, 505, 504, 503, 502, 501, 500, 507, 506, 549, 548, 509, 508, 533, 532, 543, 542, 545, 559, 558, 561, 560, 563, 562, 471, 183, 185, 187, 186, 189, 188, 206, 205, 204, 226, 215, 214, 213, 218, 257, 258, 259,
        265, 264, 282, 283, 292, 321, 316, 339, 338, 350, 354, 361, 374, 376, 405, 421, 420, 423, 422, 431, 430, 440, 468, 467, 466, 469, 480, 479, 478, 481, 524, 523, 525, 528, 553, 552, 565, 564, 571, 579, 578, 580, 135, 142, 141, 589, 534, 85, 86, 87, 71, 225, 224, 223, 357, 356, 355, 380, 379, 378, 159, 158, 307, 306, 396, 395, 499, 498, 518, 517, 512, 511, 516, 515, 514, 513, 256, 174, 173, 170, 169, 573, 572, 281, 280, 275, 274, 335, 334, 404, 403, 415, 414, 577, 576, 329, 222, 221, 220, 269,
        268, 293, 535, 367, 366, 172, 171, 180, 179, 411, 410, 176, 175, 178, 177, 253, 252, 255, 254, 318, 317, 320, 319, 417, 416, 419, 418, 450, 449, 452, 451, 520, 519, 522, 521, 464, 463, 483, 482, 261, 260, 289, 288, 377, 227, 427, 426, 567, 566, 155, 154, 249, 248, 409, 408, 413, 412, 392, 391, 407, 406, 547, 546, 358, 381, 485, 326, 219, 437, 168, 203, 202, 351, 484, 465, 568, 591, 590, 184, 510, 529, 251, 250, 331, 330, 436, 435, 448, 447, 551, 550
    };
    // clang-format on

    bool use_internal_sort_order =
        (string_compare_method == STRING_COMPARE_CORE) || (string_compare_method == STRING_COMPARE_CORE_SIMILAR);

    if (use_internal_sort_order) {
        // Core-only method. Compares in us_EN locale (sorting may be slightly inaccurate in some countries). Will
        // return arbitrary return value for invalid utf8 (silent error treatment). If one or both strings have
        // unicodes beyond 'Latin Extended 2' (0...591), then the strings are compared by unicode value.
        uint32_t char1;
        uint32_t char2;
        do {
            size_t remaining1 = string1.size() - (s1 - string1.data());
            size_t remaining2 = string2.size() - (s2 - string2.data());

            if ((remaining1 == 0) != (remaining2 == 0)) {
                // exactly one of the strings have ended (not both or none; xor)
                return (remaining1 == 0);
            }
            else if (remaining2 == 0 && remaining1 == 0) {
                // strings are identical
                return false;
            }

            // invalid utf8
            if (remaining1 < sequence_length(s1[0]) || remaining2 < sequence_length(s2[0]))
                return false;

            char1 = utf8value(s1);
            char2 = utf8value(s2);

            if (char1 == char2) {
                // Go to next characters for both strings
                s1 += sequence_length(s1[0]);
                s2 += sequence_length(s2[0]);
            }
            else {
                // Test if above Latin Extended B
                if (char1 > last_latin_extended_2_unicode || char2 > last_latin_extended_2_unicode)
                    return char1 < char2;

                const uint32_t* internal_collation_order = collation_order_core;
                if (string_compare_method == STRING_COMPARE_CORE_SIMILAR) {
                    internal_collation_order = collation_order_core_similar;
                }
                uint32_t value1 = internal_collation_order[char1];
                uint32_t value2 = internal_collation_order[char2];

                return value1 < value2;
            }

        } while (true);
    }
    else if (string_compare_method == STRING_COMPARE_CPP11) {
        // C++11. Precise sorting in user's current locale. Arbitrary return value (silent error) for invalid utf8
        std::wstring wstring1 = utf8_to_wstring(string1);
        std::wstring wstring2 = utf8_to_wstring(string2);
        std::locale l = std::locale("");
        bool ret = l(wstring1, wstring2);
        return ret;
    }
    else if (string_compare_method == STRING_COMPARE_CALLBACK) {
        // Callback method
        bool ret = string_compare_callback(s1, s2);
        return ret;
    }

    REALM_ASSERT(false);
    return false;
}


// Here is a version for Windows that may be closer to what is ultimately needed.
/*
bool case_map(const char* begin, const char* end, StringBuffer& dest, bool upper)
{
const int wide_buffer_size = 32;
wchar_t wide_buffer[wide_buffer_size];

dest.resize(end-begin);
size_t dest_offset = 0;

for (;;) {
int num_out;

// Decode
{
size_t num_in = end - begin;
if (size_t(32) <= num_in) {
num_out = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, begin, 32, wide_buffer, wide_buffer_size);
if (num_out != 0) {
begin += 32;
goto convert;
}
if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) return false;
}
if (num_in == 0) break;
int n = num_in < size_t(8) ? int(num_in) : 8;
num_out = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, begin, n, wide_buffer, wide_buffer_size);
if (num_out != 0) {
begin += n;
goto convert;
}
return false;
}

convert:
if (upper) {
for (int i=0; i<num_out; ++i) {
CharUpperW(wide_buffer + i);
}
}
else {
for (int i=0; i<num_out; ++i) {
CharLowerW(wide_buffer + i);
}
}

encode:
{
size_t free = dest.size() - dest_offset;
if (int_less_than(std::numeric_limits<int>::max(), free)) free = std::numeric_limits<int>::max();
int n = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wide_buffer, num_out,
dest.data() + dest_offset, int(free), 0, 0);
if (i != 0) {
dest_offset += n;
continue;
}
if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) return false;
size_t dest_size = dest.size();
if (int_multiply_with_overflow_detect(dest_size, 2)) {
if (dest_size == std::numeric_limits<size_t>::max()) return false;
dest_size = std::numeric_limits<size_t>::max();
}
dest.resize(dest_size);
goto encode;
}
}

dest.resize(dest_offset);
return true;
}
*/


// Converts UTF-8 source into upper or lower case. This function
// preserves the byte length of each UTF-8 character in following way:
// If an output character differs in size, it is simply substituded by
// the original character. This may of course give wrong search
// results in very special cases. Todo.
util::Optional<std::string> case_map(StringData source, bool upper)
{
    std::string result;
    result.resize(source.size());

#if defined(_WIN32)
    const char* begin = source.data();
    const char* end = begin + source.size();
    auto output = result.begin();
    while (begin != end) {
        int n = static_cast<int>(sequence_length(*begin));
        if (n == 0 || end - begin < n)
            return util::none;

        wchar_t tmp[2]; // FIXME: Why no room for UTF-16 surrogate

        int n2 = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, begin, n, tmp, 1);
        if (n2 == 0)
            return util::none;

        REALM_ASSERT(n2 == 1);
        tmp[n2] = 0;

        // Note: If tmp[0] == 0, it is because the string contains a
        // null-chacarcter, which is perfectly fine.

#if REALM_UWP
        tmp[0] = unicode_case_convert(tmp[0], upper);
#else
        if (upper)
            CharUpperW(static_cast<LPWSTR>(tmp));
        else
            CharLowerW(static_cast<LPWSTR>(tmp));
#endif

        // FIXME: The intention is to use flag 'WC_ERR_INVALID_CHARS'
        // to catch invalid UTF-8. Even though the documentation says
        // unambigously that it is supposed to work, it doesn't. When
        // the flag is specified, the function fails with error
        // ERROR_INVALID_FLAGS.
        DWORD flags = 0;
        int n3 = WideCharToMultiByte(CP_UTF8, flags, tmp, 1, &*output, static_cast<int>(end - begin), 0, 0);
        if (n3 == 0 && GetLastError() != ERROR_INSUFFICIENT_BUFFER)
            return util::none;

        if (n3 != n) {
            std::copy_n(begin, n, output); // Cannot handle different size, copy source
        }

        begin += n;
        output += n;
    }

    return result;
#else
    // FIXME: Implement this! Note that this is trivial in C++11 due
    // to its built-in support for UTF-8. In C++03 it is trivial when
    // __STDC_ISO_10646__ is defined. Also consider using ICU. Maybe
    // GNU has something to offer too.

    // For now we handle just the ASCII subset
    typedef std::char_traits<char> traits;
    if (upper) {
        size_t n = source.size();
        for (size_t i = 0; i < n; ++i) {
            char c = source[i];
            if (traits::lt(0x60, c) && traits::lt(c, 0x7B))
                c = traits::to_char_type(traits::to_int_type(c) - 0x20);
            result[i] = c;
        }
    }
    else { // lower
        size_t n = source.size();
        for (size_t i = 0; i < n; ++i) {
            char c = source[i];
            if (traits::lt(0x40, c) && traits::lt(c, 0x5B))
                c = traits::to_char_type(traits::to_int_type(c) + 0x20);
            result[i] = c;
        }
    }

    return result;
#endif
}

std::string case_map(StringData source, bool upper, IgnoreErrorsTag)
{
    return case_map(source, upper).value_or("");
}

// If needle == haystack, return true. NOTE: This function first
// performs a case insensitive *byte* compare instead of one whole
// UTF-8 character at a time. This is very fast, but not enough to
// guarantee that the strings are identical, so we need to finish off
// with a slower but rigorous comparison. The signature is similar in
// spirit to std::equal().
bool equal_case_fold(StringData haystack, const char* needle_upper, const char* needle_lower)
{
    for (size_t i = 0; i != haystack.size(); ++i) {
        char c = haystack[i];
        if (needle_lower[i] != c && needle_upper[i] != c)
            return false;
    }

    const char* begin = haystack.data();
    const char* end = begin + haystack.size();
    const char* i = begin;
    while (i != end) {
        if (!equal_sequence(i, end, needle_lower + (i - begin)) &&
            !equal_sequence(i, end, needle_upper + (i - begin)))
            return false;
    }
    return true;
}


// Test if needle is a substring of haystack. The signature is similar
// in spirit to std::search().
size_t search_case_fold(StringData haystack, const char* needle_upper, const char* needle_lower, size_t needle_size)
{
    // FIXME: This solution is very inefficient. Consider deploying the Boyer-Moore algorithm.
    size_t i = 0;
    while (needle_size <= haystack.size() - i) {
        if (equal_case_fold(haystack.substr(i, needle_size), needle_upper, needle_lower)) {
            return i;
        }
        ++i;
    }
    return haystack.size(); // Not found
}

/// This method takes an array that maps chars (both upper- and lowercase) to distance that can be moved
/// (and zero for chars not in needle), allowing the method to apply Boyer-Moore for quick substring search
/// The map is calculated in the StringNode<ContainsIns> class (so it can be reused across searches)
bool contains_ins(StringData haystack, const char* needle_upper, const char* needle_lower, size_t needle_size, const std::array<uint8_t, 256> &charmap)
{
    if (needle_size == 0)
        return haystack.size() != 0;
    
    // Prepare vars to avoid lookups in loop
    size_t last_char_pos = needle_size-1;
    unsigned char lastCharU = needle_upper[last_char_pos];
    unsigned char lastCharL = needle_lower[last_char_pos];
    
    // Do Boyer-Moore search
    size_t p = last_char_pos;
    while (p < haystack.size()) {
        unsigned char c = haystack.data()[p]; // Get candidate for last char
        
        if (c == lastCharU || c == lastCharL) {
            StringData candidate = haystack.substr(p-needle_size+1, needle_size);
            if (equal_case_fold(candidate, needle_upper, needle_lower))
                return true; // text found!
        }
        
        // If we don't have a match, see how far we can move char_pos
        if (charmap[c] == 0)
            p += needle_size; // char was not present in search string
        else
            p += charmap[c];
    }
    
    return false;
}

// pre-declaration
bool matchlike_ins(const StringData& text, const StringData& pattern_upper, const StringData& pattern_lower) noexcept;

bool matchlike_ins(const StringData& text, const StringData& pattern_upper, const StringData& pattern_lower) noexcept
{
    std::vector<size_t> textpos;
    std::vector<size_t> patternpos;
    size_t p1 = 0; // position in text (haystack)
    size_t p2 = 0; // position in pattern (needle)

    while (true) {
        if (p1 == text.size()) {
            if (p2 == pattern_lower.size())
                return true;
            if (p2 == pattern_lower.size() - 1 && pattern_lower[p2] == '*')
                return true;
            goto no_match;
        }
        if (p2 == pattern_lower.size())
            goto no_match;

        if (pattern_lower[p2] == '*') {
            textpos.push_back(p1);
            patternpos.push_back(++p2);
            continue;
        }
        if (pattern_lower[p2] == '?') {
            // utf-8 encoded characters may take up multiple bytes
            if ((text[p1] & 0x80) == 0) {
                ++p1;
                ++p2;
                continue;
            }
            else {
                size_t p = 1;
                while (p1 + p != text.size() && (text[p1 + p] & 0xc0) == 0x80)
                    ++p;
                p1 += p;
                ++p2;
                continue;
            }
        }

        if (pattern_lower[p2] == text[p1] || pattern_upper[p2] == text[p1]) {
            ++p1;
            ++p2;
            continue;
        }

    no_match:
        if (textpos.empty())
            return false;
        else {
            if (p1 == text.size()) {
                textpos.pop_back();
                patternpos.pop_back();

                if (textpos.empty())
                    return false;

                p1 = textpos.back();
            }
            else {
                p1 = textpos.back();
                textpos.back() = ++p1;
            }
            p2 = patternpos.back();
        }
    }
}

bool string_like_ins(StringData text, StringData upper, StringData lower) noexcept
{
    if (text.is_null() || lower.is_null()) {
        return (text.is_null() && lower.is_null());
    }

    return matchlike_ins(text, lower, upper);
}

bool string_like_ins(StringData text, StringData pattern) noexcept
{
    if (text.is_null() || pattern.is_null()) {
        return (text.is_null() && pattern.is_null());
    }

    std::string upper = case_map(pattern, true, IgnoreErrors);
    std::string lower = case_map(pattern, false, IgnoreErrors);

    return matchlike_ins(text, lower.c_str(), upper.c_str());
}

} // namespace realm


/*
// This is source code for generating the table in utf8_compare() from an allkey.txt file:

// Unicodes up to and including 'Latin Extended 2' (0...591)

std::vector<int64_t> order;
order.resize(last_latin_extended_2_unicode + 1);
std::string line;
std::ifstream myfile("d:/allkeys.txt");

// Read header text
for (size_t t = 0; t < 19; t++)
    getline(myfile, line);

// Read payload
for (size_t entry = 0; getline(myfile, line); entry++)
{
    string str = line.substr(0, 4);
    int64_t unicode = std::stoul(str, nullptr, 16);
    if (unicode < order.size())
    order[unicode] = entry;
}

// Emit something that you can copy/paste into the Core source code in unicode.cpp
cout << "static const uint32_t collation_order[] = {";
for (size_t t = 0; t < order.size(); t++) {
    if (t > 0 && t % 40 == 0)
        cout << "\n";
    cout << order[t] << (t + 1 < order.size() ? ", " : "");
}

cout << "};";
myfile.close();
*/
