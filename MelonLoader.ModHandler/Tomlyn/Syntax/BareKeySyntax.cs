// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
using System;
using MelonLoader.Tomlyn.Text;

namespace MelonLoader.Tomlyn.Syntax
{
    /// <summary>
    /// A TOML bare key syntax node.
    /// </summary>
    public sealed class BareKeySyntax : BareKeyOrStringValueSyntax
    {
        private SyntaxToken _key;

        /// <summary>
        /// Creates a new instance of a <see cref="BareKeySyntax"/>
        /// </summary>
        public BareKeySyntax() : base(SyntaxKind.BasicKey)
        {
        }

        /// <summary>
        /// Creates a new instance of a <see cref="BareKeySyntax"/>
        /// </summary>
        /// <param name="name">The name used for this key</param>
        public BareKeySyntax(string name) : this()
        {
            if (!IsBareKey(name)) throw new ArgumentOutOfRangeException($"The key `{name}` does not contain valid characters [A-Za-z0-9_\\-]");
            Key = new SyntaxToken(TokenKind.BasicKey, name);
        }

        /// <summary>
        /// A textual representation of the key
        /// </summary>
        public SyntaxToken Key
        {
            get => _key;
            set => ParentToThis(ref _key, value, TokenKind.BasicKey);
        }

        public override int ChildrenCount => 1;

        public override void Accept(SyntaxVisitor visitor)
        {
            visitor.Visit(this);
        }

        protected override SyntaxNode GetChildrenImpl(int index)
        {
            return Key;
        }
        
        public static bool IsBareKey(string name)
        {
            if (name == null) throw new ArgumentNullException(nameof(name));
            if (name.Length == 0 || string.IsNullOrEmpty(name)) return false;
            foreach (var c in name)
            {
                if (!CharHelper.IsKeyContinue(c))
                {
                    return false;
                }
            }
            return true;
        }
    }
}