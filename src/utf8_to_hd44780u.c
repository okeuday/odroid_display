/* -*-Mode:C;coding:utf-8;tab-width:4;c-basic-offset:4;indent-tabs-mode:()-*-
 * ex: set ft=c fenc=utf-8 sts=4 ts=4 sw=4 et nomod:
 *
 * MIT License
 *
 * Copyright (c) 2015-2017 Michael Truog <mjtruog at protonmail dot com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/* HD44780U ROM Code A00 character set conversion
 */
unsigned char utf8_to_hd44780u_0(unsigned char const ** const str)
{
    unsigned char const c0 = **str;
    unsigned char c1, c2;
    unsigned char const missing = 0x20;
    unsigned char const invalid = 0xff;
    ++(*str);
    if (c0 == 0x0)
    {
        return 0x0;
    }
    else if (c0 < 0x20)
    {
        return missing;
    }
    else if (c0 <= 0x7d)
    {
        if (c0 == 0x5c) /* '\' is N/A */
            return missing;
        return c0;
    }
    else if (c0 == 0xc2)
    {
        c1 = **str;
        ++(*str);
        switch (c1)
        {
            case 0xa0: /* ' ' */
                return 0x20;
            case 0xa2: /* ¢ */
                return 0xec;
            case 0xa5: /* ¥ */
                return 0x5c;
            case 0xb0: /* ° */
                return 0xdf;
            case 0xb5: /* µ */
                return 0xe4;
            case 0xb7: /* · */
                return 0xa5;
            default:
                return invalid;
        }
    }
    else if (c0 == 0xc3)
    {
        c1 = **str;
        ++(*str);
        switch (c1)
        {
            case 0xa4: /* ä */
                return 0xe1;
            case 0xb1: /* ñ */
                return 0xee;
            case 0xb6: /* ö */
                return 0xef;
            case 0xb7: /* ÷ */
                return 0xfd;
            case 0xbc: /* ü */
                return 0xf5;
            default:
                return invalid;
        }
    }
    else if (c0 == 0xc4 ||
             c0 == 0xc5 ||
             c0 == 0xc6 ||
             c0 == 0xc7 ||
             c0 == 0xc8 ||
             c0 == 0xc9 ||
             c0 == 0xca ||
             c0 == 0xcb ||
             c0 == 0xcc ||
             c0 == 0xcd)
    {
        ++(*str);
        return invalid;
    }
    else if (c0 == 0xce)
    {
        c1 = **str;
        ++(*str);
        switch (c1)
        {
            case 0xa3: /* Σ (upper greek sigma) */
                return 0xf6;
            case 0xa9: /* Ω (upper greek omega) */
                return 0xf4;
            case 0xb1: /* α (lower greek alpha) */
                return 0xe0;
            case 0xb2: /* β (lower greek beta) */
                return 0xe2;
            case 0xb4: /* γ (lower greek delta) */
                return 0xe5;
            case 0xb5: /* ε (lower greek epsilon) */
                return 0xe3;
            case 0xb8: /* θ (lower greek theta) */
                return 0xf2;
            case 0xbc: /* μ (lower greek mu) */
                return 0xe4;
            default:
                return invalid;
        }
    }
    else if (c0 == 0xcf)
    {
        c1 = **str;
        ++(*str);
        switch (c1)
        {
            case 0x80: /* π (lower greek pi) */
                return 0xf7;
            case 0x81: /* ρ (lower greek rho) */
                return 0xe6;
            default:
                return invalid;
        }
    }
    else if (c0 == 0xd0 ||
             c0 == 0xd1 ||
             c0 == 0xd2 ||
             c0 == 0xd3 ||
             c0 == 0xd4 ||
             c0 == 0xd5 ||
             c0 == 0xd6 ||
             c0 == 0xd7 ||
             c0 == 0xd8 ||
             c0 == 0xd9 ||
             c0 == 0xda ||
             c0 == 0xdb ||
             c0 == 0xdc ||
             c0 == 0xdd ||
             c0 == 0xde ||
             c0 == 0xdf)
    {
        ++(*str);
        return invalid;
    }
    else if (c0 == 0xe0 ||
             c0 == 0xe1)
    {
        ++(*str);
        ++(*str);
        return invalid;
    }
    else if (c0 == 0xe2)
    {
        c1 = **str;
        ++(*str);
        c2 = **str;
        ++(*str);
        if (c1 == 0x82 && c2 == 0xa4)
        {
            return 0xed; /* ₤ (lira currency) */
        }
        else if (c1 == 0x86)
        {
            switch (c2)
            {
                case 0x90: /* ← (left arrow) */
                    return 0x7f;
                case 0x92: /* → (right arrow) */
                    return 0x7e;
                default:
                    return invalid;
            }
        }
        else if (c1 == 0x88)
        {
            switch (c2)
            {
                case 0x91: /* ∑ (n-ary summation (upper greek sigma)) */
                    return 0xf6;
                case 0x9a: /* √ (square root) */
                    return 0xe8;
                case 0x9e: /* ∞ (infinity) */
                    return 0xf3;
                default:
                    return invalid;
            }
        }
        else if (c1 == 0x8c)
        {
            switch (c2)
            {
                case 0x9c: /* ⌜ (top left corner) */
                    return 0xa2;
                case 0x9f: /* ⌟ (bottom right corner) */
                    return 0xa3;
                default:
                    return invalid;
            }
        }
        else if (c1 == 0x96 && c2 == 0x88)
        {
            return 0xff; /* █ (full block) */
        }
        return invalid;
    }
    else if (c0 == 0xe3 ||
             c0 == 0xe4 ||
             c0 == 0xe5 ||
             c0 == 0xe6 ||
             c0 == 0xe7 ||
             c0 == 0xe8 ||
             c0 == 0xe9 ||
             c0 == 0xea ||
             c0 == 0xeb ||
             c0 == 0xec ||
             c0 == 0xed ||
             c0 == 0xee ||
             c0 == 0xef)
    {
        ++(*str);
        ++(*str);
        return invalid;
    }
    else if (c0 == 0xf0 ||
             c0 == 0xf1 ||
             c0 == 0xf2 ||
             c0 == 0xf3 ||
             c0 == 0xf4)
    {
        ++(*str);
        ++(*str);
        ++(*str);
        return invalid;
    }
    return missing; /* not UTF-8 data (or a continuation byte) */
}

