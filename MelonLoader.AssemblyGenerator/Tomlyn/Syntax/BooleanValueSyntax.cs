// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
namespace MelonLoader.Tomlyn.Syntax
{
    /// <summary>
    /// A boolean TOML value syntax node.
    /// </summary>
    public sealed class BooleanValueSyntax : ValueSyntax
    {
        private SyntaxToken _token;
        private bool _value;

        /// <summary>
        /// Creates an instance of a <see cref="BooleanValueSyntax"/>
        /// </summary>
        public BooleanValueSyntax() : base(SyntaxKind.Boolean)
        {
        }

        /// <summary>
        /// Creates an instance of a <see cref="BooleanValueSyntax"/>
        /// </summary>
        /// <param name="value">The boolean value</param>
        public BooleanValueSyntax(bool value) : this()
        {
            var kind = value ? TokenKind.True : TokenKind.False;
            Token = new SyntaxToken(kind, kind.ToText());
        }

        /// <summary>
        /// The boolean token value (true or false)
        /// </summary>
        public SyntaxToken Token
        {
            get => _token;
            set { ParentToThis(ref _token, value, TokenKind.True, TokenKind.False); }
        }

        /// <summary>
        /// The boolean parsed value.
        /// </summary>
        public bool Value
        {
            get => _value;
            set => _value = value;
        }

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