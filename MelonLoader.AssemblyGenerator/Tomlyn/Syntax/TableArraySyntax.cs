// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
using System;

namespace MelonLoader.Tomlyn.Syntax
{
    public sealed class TableArraySyntax : TableSyntaxBase
    {
        public TableArraySyntax() : base(SyntaxKind.TableArray)
        {
        }

        public TableArraySyntax(string name) : this()
        {
            if (name == null) throw new ArgumentNullException(nameof(name));
            Name = new KeySyntax(name);
            OpenBracket = SyntaxFactory.Token(TokenKind.OpenBracketDouble);
            CloseBracket = SyntaxFactory.Token(TokenKind.CloseBracketDouble);
            EndOfLineToken = SyntaxFactory.NewLine();
        }

        public TableArraySyntax(KeySyntax name) : this()
        {
            Name = name ?? throw new ArgumentNullException(nameof(name));
            OpenBracket = SyntaxFactory.Token(TokenKind.OpenBracketDouble);
            CloseBracket = SyntaxFactory.Token(TokenKind.CloseBracketDouble);
            EndOfLineToken = SyntaxFactory.NewLine();
        }

        public override void Accept(SyntaxVisitor visitor)
        {
            visitor.Visit(this);
        }

        internal override TokenKind OpenTokenKind => TokenKind.OpenBracketDouble;

        internal override TokenKind CloseTokenKind => TokenKind.CloseBracketDouble;
    }
}