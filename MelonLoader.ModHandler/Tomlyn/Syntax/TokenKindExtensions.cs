// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
namespace MelonLoader.Tomlyn.Syntax
{
    /// <summary>
    /// Helper functions for <see cref="TokenKind"/>
    /// </summary>
    public static class TokenKindExtensions
    {
        /// <summary>
        /// Gets a textual representation of a token kind or null if not applicable (e.g TokenKind.Integer)
        /// </summary>
        /// <param name="kind">A token kind</param>
        /// <returns>A textual representation of a token kind or null if not applicable (e.g TokenKind.Integer)</returns>
        public static string ToText(this TokenKind kind)
        {
            switch (kind)
            {
                case TokenKind.Comma:
                    return ",";
                case TokenKind.Dot:
                    return ".";
                case TokenKind.Equal:
                    return "=";
                case TokenKind.OpenBracket:
                    return "[";
                case TokenKind.OpenBracketDouble:
                    return "[[";
                case TokenKind.CloseBracket:
                    return "]";
                case TokenKind.CloseBracketDouble:
                    return "]]";
                case TokenKind.OpenBrace:
                    return "{";
                case TokenKind.CloseBrace:
                    return "}";
                case TokenKind.True:
                    return "true";
                case TokenKind.False:
                    return "false";
                case TokenKind.Infinite:
                    return "inf";
                case TokenKind.PositiveInfinite:
                    return "+inf";
                case TokenKind.NegativeInfinite:
                    return "-inf";
                case TokenKind.Nan:
                    return "nan";
                case TokenKind.PositiveNan:
                    return "+nan";
                case TokenKind.NegativeNan:
                    return "-nan";
            }

            return null;
        }

        /// <summary>
        /// Checks if the specified kind is a float.
        /// </summary>
        /// <param name="kind">A token kind</param>
        /// <returns><c>true</c> if the specified kind is a float.</returns>
        public static bool IsFloat(this TokenKind kind)
        {
            switch (kind)
            {
                case TokenKind.Float:
                case TokenKind.Infinite:
                case TokenKind.PositiveInfinite:
                case TokenKind.NegativeInfinite:
                case TokenKind.Nan:
                case TokenKind.PositiveNan:
                case TokenKind.NegativeNan:
                    return true;
            }

            return false;
        }

        /// <summary>
        /// Checks if the specified kind is an integer
        /// </summary>
        /// <param name="kind">A token kind</param>
        /// <returns><c>true</c> if the specified kind is an integer.</returns>
        public static bool IsInteger(this TokenKind kind)
        {
            switch (kind)
            {
                case TokenKind.Integer:
                case TokenKind.IntegerBinary:
                case TokenKind.IntegerHexa:
                case TokenKind.IntegerOctal:
                    return true;
            }

            return false;
        }

        /// <summary>
        /// Checks if the specified kind is a datetime.
        /// </summary>
        /// <param name="kind">A token kind</param>
        /// <returns><c>true</c> if the specified kind is a datetime.</returns>
        public static bool IsDateTime(this TokenKind kind)
        {
            switch (kind)
            {
                case TokenKind.OffsetDateTime:
                case TokenKind.LocalDateTime:
                case TokenKind.LocalDate:
                case TokenKind.LocalTime:
                    return true;
            }

            return false;
        }

        /// <summary>
        /// Checks if the specified kind is a string.
        /// </summary>
        /// <param name="kind">A token kind</param>
        /// <returns><c>true</c> if the specified kind is a string</returns>
        public static bool IsString(this TokenKind kind)
        {
            switch (kind)
            {
                case TokenKind.String:
                case TokenKind.StringLiteral:
                case TokenKind.StringMulti:
                case TokenKind.StringLiteralMulti:
                    return true;
            }

            return false;
        }

        /// <summary>
        /// Checks if the specified kind is a trivia.
        /// </summary>
        /// <param name="kind">A token kind</param>
        /// <returns><c>true</c> if the specified kind is a trivia</returns>
        public static bool IsTrivia(this TokenKind kind)
        {
            switch (kind)
            {
                case TokenKind.Whitespaces:
                case TokenKind.NewLine:
                case TokenKind.Comment:
                    return true;
                default:
                    return false;
            }
        }

        /// <summary>
        /// Checks if the specified kind is a token for which <see cref="ToText"/> will return not null
        /// </summary>
        /// <param name="kind">A token kind</param>
        /// <returns><c>true</c> if the specified kind is a simple token</returns>
        public static bool IsToken(this TokenKind kind)
        {
            switch (kind)
            {
                case TokenKind.NewLine:
                case TokenKind.Comma:
                case TokenKind.Dot:
                case TokenKind.Equal:
                case TokenKind.OpenBracket:
                case TokenKind.OpenBracketDouble:
                case TokenKind.CloseBracket:
                case TokenKind.CloseBracketDouble:
                case TokenKind.OpenBrace:
                case TokenKind.CloseBrace:
                case TokenKind.True:
                case TokenKind.False:
                case TokenKind.Infinite:
                case TokenKind.PositiveInfinite:
                case TokenKind.NegativeInfinite:
                case TokenKind.Nan:
                case TokenKind.PositiveNan:
                case TokenKind.NegativeNan:
                    return true;
                default:
                    return false;
            }
        }
    }
}