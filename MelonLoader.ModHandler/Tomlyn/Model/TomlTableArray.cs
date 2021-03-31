// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
using System;
using System.Collections;
using System.Collections.Generic;

namespace MelonLoader.Tomlyn.Model
{
    /// <summary>
    /// Runtime representation of a TOML table array
    /// </summary>
    public sealed class TomlTableArray : TomlObject, IList<TomlTable>
    {
        private readonly List<TomlTable> _items;

        public TomlTableArray() : base(ObjectKind.TableArray)
        {
            _items = new List<TomlTable>();
        }

        internal TomlTableArray(int capacity) : base(ObjectKind.TableArray)
        {
            _items = new List<TomlTable>(capacity);
        }


        public List<TomlTable>.Enumerator GetEnumerator()
        {
            return _items.GetEnumerator();
        }

        IEnumerator<TomlTable> IEnumerable<TomlTable>.GetEnumerator()
        {
            return GetEnumerator();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }

        public void Add(TomlTable item)
        {
            if (item == null) throw new ArgumentNullException(nameof(item));
            _items.Add(item);
        }

        public void Clear()
        {
            _items.Clear();
        }

        public bool Contains(TomlTable item)
        {
            if (item == null) throw new ArgumentNullException(nameof(item));
            return _items.Contains(item);
        }

        public void CopyTo(TomlTable[] array, int arrayIndex)
        {
            _items.CopyTo(array, arrayIndex);
        }

        public bool Remove(TomlTable item)
        {
            if (item == null) throw new ArgumentNullException(nameof(item));
            return _items.Remove(item);
        }

        public int Count => _items.Count;
        public bool IsReadOnly => false;

        public int IndexOf(TomlTable item)
        {
            if (item == null) throw new ArgumentNullException(nameof(item));
            return _items.IndexOf(item);
        }

        public void Insert(int index, TomlTable item)
        {
            if (item == null) throw new ArgumentNullException(nameof(item));
            _items.Insert(index, item);
        }

        public void RemoveAt(int index)
        {
            _items.RemoveAt(index);
        }

        public TomlTable this[int index]
        {
            get => _items[index];
            set => _items[index] = value ?? throw new ArgumentNullException(nameof(value));
        }
    }
}