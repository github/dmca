// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
using System.Globalization;

namespace MelonLoader.Tomlyn.Syntax
{
    /// <summary>
    /// An integer TOML value syntax node.
    /// </summary>
    public sealed class IntegerValueSyntax : ValueSyntax
    {
        private SyntaxToken _token;

        /// <summary>
        /// Creates an <see cref="IntegerValueSyntax"/>
        /// </summary>
        public IntegerValueSyntax() : base(SyntaxKind.Integer)
        {
        }

        /// <summary>
        /// Creates an <see cref="IntegerValueSyntax"/> 
        /// </summary>
        /// <param name="value">The integer value</param>
        public IntegerValueSyntax(long value) : this()
        {
            Token = new SyntaxToken(TokenKind.Integer, value.ToString(CultureInfo.InvariantCulture));
            Value = value;
        }

        /// <summary>
        /// The integer token with its textual representation
        /// </summary>
        public SyntaxToken Token
        {
            get => _token;
            set => ParentToThis(ref _token, value, value != null && value.TokenKind.IsInteger(), "decimal/hex/binary/octal integer");
        }

        /// <summary>
        /// The parsed integer value
        /// </summary>
        public long Value { get; set; }

        public override void Accept(SyntaxVisitor visitor)
        {
            visitor.Visit(this);
        }
        public override int ChildrenCount => 1;

        protected override SyntaxNode GetChildrenImpl(int index)
        {
            return Token;
        }
    }
}