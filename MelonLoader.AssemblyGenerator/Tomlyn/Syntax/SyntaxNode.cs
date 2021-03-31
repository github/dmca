// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
using System;
using System.Collections.Generic;
using System.IO;
using MelonLoader.Tomlyn.Helpers;

namespace MelonLoader.Tomlyn.Syntax
{
    /// <summary>
    /// Base class used to define a TOML Syntax tree.
    /// </summary>
    public abstract class SyntaxNode : SyntaxNodeBase
    {
        protected SyntaxNode(SyntaxKind kind)
        {
            Kind = kind;
        }

        /// <summary>
        /// Gets the type of node.
        /// </summary>
        public SyntaxKind Kind { get; }

        /// <summary>
        /// Gets the leading trivia attached to this node. Might be null if no leading trivias.
        /// </summary>
        public List<SyntaxTrivia> LeadingTrivia { get; set; }

        /// <summary>
        /// Gets the trailing trivia attached to this node. Might be null if no trailing trivias.
        /// </summary>
        public List<SyntaxTrivia> TrailingTrivia { get; set; }

        /// <summary>
        /// Gets the number of children
        /// </summary>
        public abstract int ChildrenCount { get; }

        /// <summary>
        /// Gets a children at the specified index.
        /// </summary>
        /// <param name="index">Index of the children</param>
        /// <returns>A children at the specified index</returns>
        public SyntaxNode GetChildren(int index)
        {
            if (index < 0) throw ThrowHelper.GetIndexNegativeArgumentOutOfRangeException();
            if (index > ChildrenCount) throw ThrowHelper.GetIndexArgumentOutOfRangeException(ChildrenCount);
            return GetChildrenImpl(index);
        }

        /// <summary>
        /// Gets a children at the specified index.
        /// </summary>
        /// <param name="index">Index of the children</param>
        /// <returns>A children at the specified index</returns>
        /// <remarks>The index is safe to use</remarks>
        protected abstract SyntaxNode GetChildrenImpl(int index);

        public override string ToString()
        {
            var writer = new StringWriter();
            WriteTo(writer);
            return writer.ToString();
        }

        /// <summary>
        /// Writes this node to a textual TOML representation
        /// </summary>
        /// <param name="writer">A writer to receive the TOML output</param>
        public void WriteTo(TextWriter writer)
        {
            if (writer == null) throw new ArgumentNullException(nameof(writer));
            var stack = new Stack<SyntaxNode>();
            stack.Push(this);
            WriteTo(stack, writer);
        }

        private static void WriteTo(Stack<SyntaxNode> stack, TextWriter writer)
        {
            while (stack.Count > 0)
            {
                var node = stack.Pop();

                WriteTriviaTo(node.LeadingTrivia, writer);
                if (node is SyntaxToken token)
                {
                    writer.Write(token.TokenKind.ToText() ?? token.Text);
                }
                else
                {
                    int count = node.ChildrenCount;
                    for (int i = count - 1; i >= 0; i--)
                    {
                        var child = node.GetChildren(i);
                        if (child == null) continue;
                        stack.Push(child);
                    }
                }
                WriteTriviaTo(node.TrailingTrivia, writer);
            }
        }

        private static void WriteTriviaTo(List<SyntaxTrivia> trivias, TextWriter writer)
        {
            if (trivias == null) return;
            foreach (var trivia in trivias)
            {
                if (trivia == null) continue;
                writer.Write(trivia.Text);
            }
        }

        /// <summary>
        /// Helper method to deparent/parent a node to this instance.
        /// </summary>
        /// <typeparam name="TSyntaxNode">Type of the node</typeparam>
        /// <param name="set">The previous child node parented to this instance</param>
        /// <param name="node">The new child node to parent to this instance</param>
        protected void ParentToThis<TSyntaxNode>(ref TSyntaxNode set, TSyntaxNode node) where TSyntaxNode : SyntaxNode
        {
            if (node?.Parent != null) throw ThrowHelper.GetExpectingNoParentException();
            if (set != null)
            {
                set.Parent = null;
            }
            if (node != null)
            {
                node.Parent = this;
            }
            set = node;
        }

        /// <summary>
        /// Helper method to deparent/parent a <see cref="SyntaxToken"/> to this instance with an expected kind of token.
        /// </summary>
        /// <typeparam name="TSyntaxNode">Type of the node</typeparam>
        /// <param name="set">The previous child node parented to this instance</param>
        /// <param name="node">The new child node to parent to this instance</param>
        /// <param name="expectedKind">The expected kind of token</param>
        protected void ParentToThis<TSyntaxNode>(ref TSyntaxNode set, TSyntaxNode node, TokenKind expectedKind) where TSyntaxNode : SyntaxToken
        {
            ParentToThis(ref set, node, node.TokenKind == expectedKind, expectedKind);
        }

        /// <summary>
        /// Helper method to deparent/parent a <see cref="SyntaxToken"/> to this instance with an expected kind of token condition.
        /// </summary>
        /// <typeparam name="TSyntaxNode">Type of the node</typeparam>
        /// <typeparam name="TExpected">The type of message</typeparam>
        /// <param name="set">The previous child node parented to this instance</param>
        /// <param name="node">The new child node to parent to this instance</param>
        /// <param name="expectedKindSuccess">true if kind is matching, false otherwise</param>
        /// <param name="expectedMessage">The message to display if the kind is not matching</param>
        protected void ParentToThis<TSyntaxNode, TExpected>(ref TSyntaxNode set, TSyntaxNode node, bool expectedKindSuccess, TExpected expectedMessage) where TSyntaxNode : SyntaxToken
        {
            if (node != null && !expectedKindSuccess) throw new InvalidOperationException($"Unexpected node kind `{node.TokenKind}` while expecting `{expectedMessage}`");
            ParentToThis(ref set, node);
        }

        /// <summary>
        /// Helper method to deparent/parent a <see cref="SyntaxToken"/> to this instance with an expected kind of token.
        /// </summary>
        /// <typeparam name="TSyntaxNode">Type of the node</typeparam>
        /// <param name="set">The previous child node parented to this instance</param>
        /// <param name="node">The new child node to parent to this instance</param>
        /// <param name="expectedKind1">The expected kind of token (option1)</param>
        /// <param name="expectedKind2">The expected kind of token (option2)</param>
        protected void ParentToThis<TSyntaxNode>(ref TSyntaxNode set, TSyntaxNode node, TokenKind expectedKind1, TokenKind expectedKind2) where TSyntaxNode : SyntaxToken
        {
            ParentToThis(ref set, node, node.TokenKind == expectedKind1 || node.TokenKind == expectedKind2, new ExpectedTuple2<TokenKind, TokenKind>(expectedKind1, expectedKind2));
        }

        private readonly struct ExpectedTuple2<T1, T2>
        {
            public ExpectedTuple2(T1 value1, T2 value2)
            {
                Value1 = value1;
                Value2 = value2;
            }

            public readonly T1 Value1;

            public readonly T2 Value2;

            public override string ToString()
            {
                return $"`{Value1}` or `{Value2}`";
            }
        }
    }
}