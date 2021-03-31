// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
using System;
using System.Collections;
using System.Collections.Generic;
using MelonLoader.Tomlyn.Syntax;

namespace MelonLoader.Tomlyn.Model
{
    /// <summary>
    /// Runtime representation of a TOML table
    /// </summary>
    /// <remarks>
    /// This object keep the order of the inserted key=values
    /// </remarks>
    public sealed class TomlTable : TomlObject, IDictionary<string, object>
    {
        // TODO: optimize the internal by avoiding two structures
        private readonly List<KeyValuePair<string, TomlObject>> _order;
        private readonly Dictionary<string, TomlObject> _map;

        /// <summary>
        /// Creates an instance of a <see cref="TomlTable"/>
        /// </summary>
        public TomlTable() : base(ObjectKind.Table)
        {
            _order = new List<KeyValuePair<string, TomlObject>>();
            _map = new Dictionary<string, TomlObject>();
        }

        public IEnumerator<KeyValuePair<string, object>> GetEnumerator()
        {
            foreach (var keyPair in _order)
            {
                yield return new KeyValuePair<string, object>(keyPair.Key, ToObject(keyPair.Value));
            }
        }


        public IEnumerable<KeyValuePair<string, TomlObject>> GetTomlEnumerator()
        {
            foreach (var keyPair in _order)
            {
                yield return new KeyValuePair<string, TomlObject>(keyPair.Key, keyPair.Value);
            }
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }

        public void Add(KeyValuePair<string, object> item)
        {
            Add(item.Key, item.Value);
        }

        public void Clear()
        {
            _map.Clear();
            _order.Clear();
        }

        public bool Contains(KeyValuePair<string, object> item)
        {
            throw new NotSupportedException();
        }

        public void CopyTo(KeyValuePair<string, object>[] array, int arrayIndex)
        {
            throw new NotSupportedException();
        }

        public bool Remove(KeyValuePair<string, object> item)
        {
            throw new NotSupportedException();
        }

        public int Count => _map.Count;

        public bool IsReadOnly => false;

        public void Add(string key, object value)
        {
            if (value == null) throw new ArgumentNullException(nameof(value));
            var toml = ToTomlObject(value);
            _map.Add(key, toml);
            _order.Add(new KeyValuePair<string, TomlObject>(key, toml));
        }

        public bool ContainsKey(string key)
        {
            return _map.ContainsKey(key);
        }

        public bool Remove(string key)
        {
            if (_map.Remove(key))
            {
                for (int i = _order.Count - 1; i >= 0; i--)
                {
                    if (_order[i].Key == key)
                    {
                        _order.RemoveAt(i);
                        break;
                    }
                }
                return true;
            }

            return false;
        }

        public bool TryGetValue(string key, out object value)
        {
            TomlObject node;
            if (_map.TryGetValue(key, out node))
            {
                value = ToObject(node);
                return true;
            }

            value = null;
            return false;
        }

        public bool TryGetToml(string key, out TomlObject value)
        {
            return _map.TryGetValue(key, out value);
        }

        public object this[string key]
        {
            get => ToObject(_map[key]);
            set
            {
                // If the value exist already, try to create it
                if (_map.TryGetValue(key, out var node))
                {
                    node = UpdateObject(node, value);
                    _map[key] = node;
                }
                else
                {
                    Add(key, value);
                }
            }
        }

        public ICollection<string> Keys
        {
            get
            {
                var list = new List<string>();
                foreach (var valuePair in _order)
                {
                    list.Add(valuePair.Key);
                }
                return list;
            }
        }

        public ICollection<object> Values
        {
            get
            {
                var list = new List<object>();
                foreach (var valuePair in _order)
                {
                    list.Add(ToObject(valuePair.Value));
                }
                return list;
            }
        }
        
        public static TomlTable From(DocumentSyntax documentSyntax)
        {
            if (documentSyntax == null) throw new ArgumentNullException(nameof(documentSyntax));
            if (documentSyntax.HasErrors) throw new InvalidOperationException($"The document has errors: {documentSyntax.Diagnostics}");
            var root = new TomlTable();
            var syntaxTransform = new SyntaxTransform(root);
            syntaxTransform.Visit(documentSyntax);
            return root;
        }
    }
}