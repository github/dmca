// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
using System;

namespace MelonLoader.Tomlyn.Syntax
{
    /// <summary>
    /// A TOML key = value syntax node.
    /// </summary>
    public sealed class KeyValueSyntax : SyntaxNode
    {
        private KeySyntax _key;
        private SyntaxToken _equalToken;
        private ValueSyntax _value;
        private SyntaxToken _endOfLineToken;

        /// <summary>
        /// Creates an instance of <see cref="KeyValueSyntax"/>
        /// </summary>
        public KeyValueSyntax() : base(SyntaxKind.KeyValue)
        {
        }

        /// <summary>
        /// Creates an instance of <see cref="KeyValueSyntax"/>
        /// </summary>
        /// <param name="key">The key</param>
        /// <param name="value">The value</param>
        public KeyValueSyntax(string key, ValueSyntax value) : this()
        {
            if (key == null) throw new ArgumentNullException(nameof(key));
            Key = new KeySyntax(key);
            Value = value ?? throw new ArgumentNullException(nameof(value));
            EqualToken = SyntaxFactory.Token(TokenKind.Equal);
            EqualToken.AddLeadingWhitespace();
            EqualToken.AddTrailingWhitespace();
            EndOfLineToken = SyntaxFactory.NewLine();
        }

        /// <summary>
        /// Creates an instance of <see cref="KeyValueSyntax"/>
        /// </summary>
        /// <param name="key">The key</param>
        /// <param name="value">The value</param>
        public KeyValueSyntax(KeySyntax key, ValueSyntax value) : this()
        {
            Key = key ?? throw new ArgumentNullException(nameof(key));
            Value = value ?? throw new ArgumentNullException(nameof(value));
            EqualToken = SyntaxFactory.Token(TokenKind.Equal);
            EqualToken.AddLeadingWhitespace();
            EqualToken.AddTrailingWhitespace();
            EndOfLineToken = SyntaxFactory.NewLine();
        }


        /// <summary>
        /// Gets or sets the key.
        /// </summary>
        public KeySyntax Key
        {
            get => _key;
            set => ParentToThis(ref _key, value);
        }

        /// <summary>
        /// Gets or sets the `=` token
        /// </summary>
        public SyntaxToken EqualToken
        {
            get => _equalToken;
            set => ParentToThis(ref _equalToken, value, TokenKind.Equal);
        }

        /// <summary>
        /// Gets or sets the value
        /// </summary>
        public ValueSyntax Value
        {
            get => _value;
            set => ParentToThis(ref _value, value);
        }

        /// <summary>
        /// Gets or sets the new-line token.
        /// </summary>
        public SyntaxToken EndOfLineToken
        {
            get => _endOfLineToken;
            set => ParentToThis(ref _endOfLineToken, value, TokenKind.NewLine, TokenKind.Eof);
        }

        public override int ChildrenCount => 4;

        public override void Accept(SyntaxVisitor visitor)
        {
            visitor.Visit(this);
        }

        protected override SyntaxNode GetChildrenImpl(int index)
        {
            switch (index)
            {
                case 0:
                    return Key;
                case 1:
                    return EqualToken;
                case 2:
                    return Value;
                default:
                    return EndOfLineToken;
            }
        }
    }
}