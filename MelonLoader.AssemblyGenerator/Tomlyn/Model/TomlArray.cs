// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
using System;
using System.Collections;
using System.Collections.Generic;

namespace MelonLoader.Tomlyn.Model
{
    /// <summary>
    /// Runtime representation of a TOML array
    /// </summary>
    public sealed class TomlArray : TomlObject, IList<object>
    {
        private readonly List<TomlObject> _items;

        public TomlArray() : base(ObjectKind.Array)
        {
            _items = new List<TomlObject>();
        }

        public TomlArray(int capacity) : base(ObjectKind.Array)
        {
            _items = new List<TomlObject>(capacity);
        }

        public IEnumerator<object> GetEnumerator()
        {
            foreach (var item in _items)
            {
                yield return ToObject(item);
            }
        }

        public IEnumerable<TomlObject> GetTomlEnumerator()
        {
            return _items;
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }

        public void Add(object item)
        {
            _items.Add(ToTomlObject(item));
        }

        public void Clear()
        {
            _items.Clear();
        }

        public bool Contains(object item)
        {
            var toml = ToTomlObject(item);
            return _items.Contains(toml);
        }

        public void CopyTo(object[] array, int arrayIndex)
        {
            throw new NotImplementedException();
        }

        public bool Remove(object item)
        {
            var toml = ToTomlObject(item);
            return _items.Remove(toml);
        }

        public int Count => _items.Count;
        public bool IsReadOnly => false;
        public int IndexOf(object item)
        {
            var toml = ToTomlObject(item);
            return _items.IndexOf(toml);
        }

        public void Insert(int index, object item)
        {
            var toml = ToTomlObject(item);
            _items.Insert(index, toml);
        }

        public void RemoveAt(int index)
        {
            _items.RemoveAt(index);
        }

        public object this[int index]
        {
            get => ToObject(_items[index]);
            set => _items[index] = ToTomlObject(value);
        }
    }
}