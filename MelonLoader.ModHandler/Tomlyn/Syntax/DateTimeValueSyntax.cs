// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
using System;
using System.Globalization;

namespace MelonLoader.Tomlyn.Syntax
{
    /// <summary>
    /// A datetime TOML value syntax node.
    /// </summary>
    public sealed class DateTimeValueSyntax : ValueSyntax
    {
        private SyntaxToken _token;

        /// <summary>
        /// Creates an instance of <see cref="DateTimeValueSyntax"/>
        /// </summary>
        /// <param name="kind">The kind of datetime</param>
        public DateTimeValueSyntax(SyntaxKind kind) : base(CheckDateTimeKind(kind))
        {
        }

        /// <summary>
        /// Creates a new instance of <see cref="DateTimeValueSyntax"/>
        /// </summary>
        /// <param name="kind"></param>
        /// <param name="value"></param>
        public DateTimeValueSyntax(SyntaxKind kind, DateTime value) : base(CheckDateTimeKind(kind))
        {
            TokenKind tokenKind = 0;
            switch (kind)
            {
                case SyntaxKind.OffsetDateTime:
                    tokenKind = TokenKind.OffsetDateTime;
                    break;
                case SyntaxKind.LocalDateTime:
                    tokenKind = TokenKind.LocalDateTime;
                    break;
                case SyntaxKind.LocalDate:
                    tokenKind = TokenKind.LocalDate;
                    break;
                case SyntaxKind.LocalTime:
                    tokenKind = TokenKind.LocalTime;
                    break;
            }

            Token = new SyntaxToken(tokenKind, ToString(kind, value));
        }

        /// <summary>
        /// Gets or sets the datetime token.
        /// </summary>
        public SyntaxToken Token
        {
            get => _token;
            set => ParentToThis(ref _token, value, value != null && value.TokenKind.IsDateTime(), $"The token kind `{value?.TokenKind}` is not a datetime token");
        }

        /// <summary>
        /// Gets or sets the parsed datetime value.
        /// </summary>
        public DateTime Value { get; set; }

        public override int ChildrenCount => 1;

        public override void Accept(SyntaxVisitor visitor)
        {
            visitor.Visit(this);
        }

        protected override SyntaxNode GetChildrenImpl(int index)
        {
            return Token;
        }

        private static SyntaxKind CheckDateTimeKind(SyntaxKind kind)
        {
            switch (kind)
            {
                case SyntaxKind.OffsetDateTime:
                case SyntaxKind.LocalDateTime:
                case SyntaxKind.LocalDate:
                case SyntaxKind.LocalTime:
                    return kind;
                default:
                    throw new ArgumentOutOfRangeException(nameof(kind), kind, null);
            }
        }

        internal static string ToString(SyntaxKind kind, DateTime value)
        {
            switch (kind)
            {
                case SyntaxKind.OffsetDateTime:
                    var time = value.ToUniversalTime();
                    if (time.Millisecond == 0) return time.ToString("yyyy-MM-dd'T'HH:mm:ssK", CultureInfo.InvariantCulture);
                    return time.ToString("yyyy-MM-dd'T'HH:mm:ss.fffK", CultureInfo.InvariantCulture);
                case SyntaxKind.LocalDateTime:
                    if (value.Millisecond == 0) return value.ToString("yyyy-MM-dd'T'HH:mm:ss", CultureInfo.InvariantCulture);
                    return value.ToString("yyyy-MM-dd'T'HH:mm:ss.fff", CultureInfo.InvariantCulture);
                case SyntaxKind.LocalDate:
                    return value.ToString("yyyy-MM-dd", CultureInfo.InvariantCulture);
                case SyntaxKind.LocalTime:
                    return value.Millisecond == 0 ? value.ToString("HH:mm:ss", CultureInfo.InvariantCulture) : value.ToString("HH:mm:ss.fff", CultureInfo.InvariantCulture);
                default:
                    throw new ArgumentOutOfRangeException(nameof(kind), kind, null);
            }
        }
    }
}