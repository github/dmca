// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
using System;

namespace MelonLoader.Tomlyn.Syntax
{
    /// <summary>
    /// A part of a TOML dotted key used by <see cref="KeySyntax"/>
    /// </summary>
    public sealed class DottedKeyItemSyntax : ValueSyntax
    {
        private SyntaxToken _dot;
        private BareKeyOrStringValueSyntax _key;

        /// <summary>
        /// Creates an instance of <see cref="DottedKeyItemSyntax"/>
        /// </summary>
        public DottedKeyItemSyntax() : base(SyntaxKind.DottedKeyItem)
        {
        }

        /// <summary>
        /// Creates an instance of <see cref="DottedKeyItemSyntax"/>
        /// </summary>
        /// <param name="key">The key used after the `.`</param>
        public DottedKeyItemSyntax(string key) : this()
        {
            if (key == null) throw new ArgumentNullException(nameof(key));
            Dot = SyntaxFactory.Token(TokenKind.Dot);
            Key = BareKeySyntax.IsBareKey(key) ? (BareKeyOrStringValueSyntax)new BareKeySyntax(key) : new StringValueSyntax(key);
        }

        /// <summary>
        /// The token `.`
        /// </summary>
        public SyntaxToken Dot
        {
            get => _dot;
            set => ParentToThis(ref _dot, value, TokenKind.Dot);
        }

        /// <summary>
        /// The following key or string node.
        /// </summary>
        public BareKeyOrStringValueSyntax Key
        {
            get => _key;
            set => ParentToThis(ref _key, value);
        }

        public override void Accept(SyntaxVisitor visitor)
        {
            visitor.Visit(this);
        }

        public override int ChildrenCount => 2;

        protected override SyntaxNode GetChildrenImpl(int index)
        {
            return index == 0 ? (SyntaxNode)Dot : Key;
        }
    }
}