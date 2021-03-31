// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
using System;
using MelonLoader.Tomlyn.Text;

namespace MelonLoader.Tomlyn.Syntax
{
    /// <summary>
    /// A string TOML syntax value node.
    /// </summary>
    public sealed class StringValueSyntax : BareKeyOrStringValueSyntax
    {
        private SyntaxToken _token;

        /// <summary>
        /// Creates a new instance of <see cref="StringValueSyntax"/>
        /// </summary>
        public StringValueSyntax() : base(SyntaxKind.String)
        {
        }

        /// <summary>
        /// Creates a new instance of <see cref="StringValueSyntax"/>
        /// </summary>
        /// <param name="text">String value used for this node</param>
        public StringValueSyntax(string text) : this()
        {
            if (text == null) throw new ArgumentNullException(nameof(text));
            Token = new SyntaxToken(TokenKind.String, $"\"{text.EscapeForToml()}\"");
            Value = text;
        }

        /// <summary>
        /// The token of the string.
        /// </summary>
        public SyntaxToken Token
        {
            get => _token;
            set => ParentToThis(ref _token, value, value != null && value.TokenKind.IsString(), "string");
        }

        /// <summary>
        /// The associated parsed string value
        /// </summary>
        public string Value { get; set; }

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