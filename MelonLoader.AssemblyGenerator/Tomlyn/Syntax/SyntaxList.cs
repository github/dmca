// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
using System;
using System.Collections;
using System.Collections.Generic;
using MelonLoader.Tomlyn.Helpers;

namespace MelonLoader.Tomlyn.Syntax
{
    /// <summary>
    /// Abstract list of <see cref="SyntaxNode"/>
    /// </summary>
    public abstract class SyntaxList : SyntaxNode
    {
        protected readonly List<SyntaxNode> Children;

        internal SyntaxList() : base(SyntaxKind.List)
        {
            Children = new List<SyntaxNode>();
        }

        public sealed override int ChildrenCount => Children.Count;

        protected override SyntaxNode GetChildrenImpl(int index)
        {
            return Children[index];
        }

        public override void Accept(SyntaxVisitor visitor)
        {
            visitor.Visit(this);
        }
    }

    /// <summary>
    /// Abstract list of <see cref="SyntaxNode"/>
    /// </summary>
    /// <typeparam name="TSyntaxNode">Type of the node</typeparam>
    public sealed class SyntaxList<TSyntaxNode> : SyntaxList, IEnumerable<TSyntaxNode> where TSyntaxNode : SyntaxNode
    {
        /// <summary>
        /// Creates an instance of <see cref="SyntaxList{TSyntaxNode}"/>
        /// </summary>
        public SyntaxList()
        {
        }

        /// <summary>
        /// Adds the specified node to this list.
        /// </summary>
        /// <param name="node">Node to add to this list</param>
        public void Add(TSyntaxNode node)
        {
            if (node == null) throw new ArgumentNullException(nameof(node));
            if (node.Parent != null) throw ThrowHelper.GetExpectingNoParentException();
            Children.Add(node);
            node.Parent = this;
        }

        public new TSyntaxNode GetChildren(int index)
        {
            return (TSyntaxNode)base.GetChildren(index);
        }

        protected override SyntaxNode GetChildrenImpl(int index)
        {
            return Children[index];
        }

        /// <summary>
        /// Removes a node at the specified index.
        /// </summary>
        /// <param name="index">Index of the node to remove</param>
        public void RemoveChildrenAt(int index)
        {
            var node = Children[index];
            Children.RemoveAt(index);
            node.Parent = null;
        }

        /// <summary>
        /// Removes the specified node instance.
        /// </summary>
        /// <param name="node">Node instance to remove</param>
        public void RemoveChildren(TSyntaxNode node)
        {
            if (node == null) throw new ArgumentNullException(nameof(node));
            if (node.Parent != this) throw new InvalidOperationException("The node is not part of this list");
            Children.Remove(node);
            node.Parent = null;
        }

        /// <summary>
        /// Gets the default enumerator.
        /// </summary>
        /// <returns>The enumerator of this list</returns>
        public Enumerator GetEnumerator()
        {
            return new Enumerator(Children);
        }

        IEnumerator<TSyntaxNode> IEnumerable<TSyntaxNode>.GetEnumerator()
        {
            return GetEnumerator();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }

        /// <summary>
        /// Enumerator of a <see cref="SyntaxList{TSyntaxNode}"/>
        /// </summary>
        public struct Enumerator : IEnumerator<TSyntaxNode>
        {
            private readonly List<SyntaxNode> _nodes;
            private int _index;

            /// <summary>
            /// Initialize an enumerator with a list of <see cref="SyntaxNode"/>
            /// </summary>
            /// <param name="nodes"></param>
            public Enumerator(List<SyntaxNode> nodes)
            {
                _nodes = nodes;
                _index = -1;
            }

            public bool MoveNext()
            {
                if (_index + 1 == _nodes.Count) return false;
                _index++;
                return true;
            }

            public void Reset()
            {
                _index = -1;
            }

            public TSyntaxNode Current
            {
                get
                {
                    if (_index < 0) throw new InvalidOperationException("MoveNext must be called before accessing Current");
                    return (TSyntaxNode)_nodes[_index];
                }
            }

            object IEnumerator.Current => Current;

            public void Dispose()
            {
            }
        }
    }
}