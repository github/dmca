// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
using System;
using System.Diagnostics;
using System.Runtime.CompilerServices;
using System.Text;

namespace MelonLoader.Tomlyn.Text
{
    internal static partial class CharHelper
    {
        public static readonly Func<char32, bool> IsHexFunc = IsHex;
        public static readonly Func<char32, bool> IsOctalFunc = IsOctal;
        public static readonly Func<char32, bool> IsBinaryFunc = IsBinary;

        public static readonly Func<char32, int> HexToDecFunc = HexToDecimal;
        public static readonly Func<char32, int> OctalToDecFunc = OctalToDecimal;
        public static readonly Func<char32, int> BinaryToDecFunc = BinaryToDecimal;

        public static bool IsControlCharacter(char32 c)
        {
            return c <= 0x1F || c == 0x7F;
        }

        public static bool IsKeyStart(char32 c)
        {
            return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c == '-' || c >= '0' && c <= '9';
        }

        public static bool IsKeyContinue(char32 c)
        {
            return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c == '-' || c >= '0' && c <= '9';
        }

        public static bool IsIdentifierStart(char32 c)
        {
            return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
        }

        public static bool IsIdentifierContinue(char32 c)
        {
            return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
        }

        public static bool IsValidUnicodeScalarValue(char32 c)
        {
            return c >= 0 && c <= 0xD7FF || c >= 0xE000 && c < 0x10FFFF;
        }

        public static bool IsDigit(char32 c)
        {
            return (c >= '0' && c <= '9');
        }

        public static bool IsDateTime(char32 c)
        {
            return IsDigit(c) || c == ':' || c == '-' || c == 'Z' || c == 'T' || c == 'z' || c == 't' || c == '+' || c == '.';
        }

        /// <summary>
        /// Escape a C# string to a TOML string
        /// </summary>
        public static string EscapeForToml(this string text)
        {
            StringBuilder builder = null;
            for (int i = 0; i < text.Length; i++)
            {
                var c = text[i];
                var str = EscapeChar(text[i]);
                if (str != null)
                {
                    if (builder == null)
                    {
                        builder = new StringBuilder(text.Length * 2);
                        builder.Append(text.Substring(0, i));
                    }
                    builder.Append(str);
                }
                else
                {
                    builder?.Append(c);
                }
            }
            return builder?.ToString() ?? text;
        }

        private static string EscapeChar(char c)
        {
            if (c < ' ' || c == '"' || c == '\\')
            {
                switch (c)
                {
                    case '\b':
                        return @"\b";
                    case '\t':
                        return @"\t";
                    case '\n':
                        return @"\n";
                    case '\f':
                        return @"\f";
                    case '\r':
                        return @"\r";
                    case '"':
                        return @"\""";
                    case '\\':
                        return @"\\";
                    default:
                        return $"\\u{(int)c:X};";
                }
            }
            return null;
        }

        /// <summary>
        /// Converts a string that may have control characters to a printable string
        /// </summary>
        public static string ToPrintableString(this string text)
        {
            StringBuilder builder = null;
            for (int i = 0; i < text.Length; i++)
            {
                var c = text[i];
                var str = text[i].ToPrintableString();
                if (str != null)
                {
                    if (builder == null)
                    {
                        builder = new StringBuilder(text.Length * 2);
                        builder.Append(text.Substring(0, i));
                    }
                    builder.Append(str);
                }
                else
                {
                    builder?.Append(c);
                }
            }
            return builder?.ToString() ?? text;
        }

        public static string ToPrintableString(this char c)
        {
            if (c < ' ' || IsWhiteSpace(c))
            {
                switch (c)
                {
                    case ' ':
                        return @" ";
                    case '\b':
                        return @"\b";
                    case '\r':
                        return @"␍";
                    case '\n':
                        return @"␤";
                    case '\t':
                        return @"\t";
                    case '\a':
                        return @"\a";
                    case '\v':
                        return @"\v";
                    case '\f':
                        return @"\f";
                    default:
                        return $"\\u{(int)c:X};";
                }
            }
            return null;
        }

        public static bool IsWhiteSpace(char32 c)
        {
            return c == ' ' || // space
                   c == '\t'; // horizontal tab
        }

        public static bool IsWhiteSpaceOrNewLine(char32 c)
        {
            return c == ' ' || // space
                   c == '\t' || // horizontal tab
                   c == '\r' || // \r
                   c == '\n'; // \n
        }

        public static bool IsNewLine(char32 c)
        {
            return c == '\r' || // \r
                   c == '\n'; // \n
        }

        public static char32? ToUtf8(byte[] buffer, ref int position)
        {
            if (position < buffer.Length)
            {
                // bytes   bits    UTF-8 representation
                // -----   ----    -----------------------------------
                // 1        7      0vvvvvvv
                // 2       11      110vvvvv 10vvvvvv
                // 3       16      1110vvvv 10vvvvvv 10vvvvvv
                // 4       21      11110vvv 10vvvvvv 10vvvvvv 10vvvvvv
                // -----   ----    -----------------------------------

                //Surrogate:
                //Real Unicode value = (HighSurrogate - 0xD800) * 0x400 + (LowSurrogate - 0xDC00) + 0x10000
                var c1 = unchecked((sbyte)buffer[position++]);
                return c1 >= 0 ? c1 : DecodeUTF8_24(buffer, ref position, c1);
            }
            position = buffer.Length;
            return null;
        }

        private static char32 DecodeUTF8_24(byte[] buffer, ref int position, sbyte c1)
        {
            int nbByte = 0;
            while (c1 < 0)
            {
                c1 = (sbyte)(c1 << 1);
                nbByte++;
            }

            if (nbByte > 4 || position + nbByte - 1 > buffer.Length)
            {
                // TODO: Throw an exception or return something else?
                throw new CharReaderException($"Invalid UTF8 character at position {position}");
            }

            int c = (c1 << (6 - nbByte)) | (buffer[position++] & 0x3f);
            if (nbByte == 2)
            {
                return c;
            }
            if (nbByte >= 3)
            {
                c = (c << 6) | (buffer[position++] & 0x3f);
            }
            if (nbByte == 4)
            {
                c = (c << 6) | (buffer[position++] & 0x3f);
            }
            return c;
        }

        public static int HexToDecimal(char32 c)
        {
            Debug.Assert(IsHex(c));
            return (c >= '0' && c <= '9') ? c - '0' : (c >= 'a' && c <= 'f') ? (c - 'a') + 10 : (c - 'A') + 10;
        }

        public static int OctalToDecimal(char32 c)
        {
            Debug.Assert(IsOctal(c));
            return c - '0';
        }

        public static int BinaryToDecimal(char32 c)
        {
            Debug.Assert(IsBinary(c));
            return c - '0';
        }

        private static bool IsHex(char32 c)
        {
            return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
        }
        private static bool IsOctal(char32 c)
        {
            return (c >= '0' && c <= '7');
        }
        private static bool IsBinary(char32 c)
        {
            return (c == '0' || c == '1');
        }

        public static void AppendUtf32(this StringBuilder builder, char32 utf32)
        {
            if (utf32 < 65536)
            {
                builder.Append((char) utf32);
                return;
            }
            utf32 -= 65536;
            builder.Append((char) (utf32 / 1024 + 55296));
            builder.Append((char) (utf32 % 1024 + 56320));
        }
    }
}