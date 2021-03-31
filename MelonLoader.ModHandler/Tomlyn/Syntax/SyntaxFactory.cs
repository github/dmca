// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
using System;
using System.Diagnostics;

namespace MelonLoader.Tomlyn.Syntax
{
    /// <summary>
    /// A factory for <see cref="SyntaxNode"/>
    /// </summary>
    public static class SyntaxFactory
    {
        /// <summary>
        /// Creates a trivia whitespace.
        /// </summary>
        /// <returns>A trivia whitespace.</returns>
        public static SyntaxTrivia Whitespace()
        {
            return new SyntaxTrivia(TokenKind.Whitespaces, " ");
        }

        /// <summary>
        /// Creates a newline trivia.
        /// </summary>
        /// <returns>A new line trivia</returns>
        public static SyntaxTrivia NewLineTrivia()
        {
            return new SyntaxTrivia(TokenKind.NewLine, "\n");
        }

        /// <summary>
        /// Creates a comment trivia.
        /// </summary>
        /// <param name="comment">A comment trivia</param>
        /// <returns>A comment trivia</returns>
        public static SyntaxTrivia Comment(string comment)
        {
            if (comment == null) throw new ArgumentNullException(nameof(comment));
            return new SyntaxTrivia(TokenKind.Comment, $"# {comment}");
        }

        /// <summary>
        /// Creates a newline token.
        /// </summary>
        /// <returns>A new line token</returns>
        public static SyntaxToken NewLine()
        {
            return new SyntaxToken(TokenKind.NewLine, "\n");
        }

        /// <summary>
        /// Creates a token from the specified token kind.
        /// </summary>
        /// <param name="kind">The token kind</param>
        /// <returns>The token</returns>
        public static SyntaxToken Token(TokenKind kind)
        {
            if (kind == TokenKind.NewLine || !kind.IsToken()) throw new ArgumentOutOfRangeException($"The token kind `{kind}` is not supported for a plain token without a predefined value");
            var text = kind.ToText();
            Debug.Assert(text != null);
            return new SyntaxToken(kind, text);
        }
    }
}