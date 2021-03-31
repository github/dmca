// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
using System;

namespace MelonLoader.Tomlyn.Model
{
    /// <summary>
    /// Base class of a TOML value (bool, string, integer, float, datetime)
    /// </summary>
    public abstract class TomlValue : TomlObject
    {
        internal TomlValue(ObjectKind kind) : base(kind)
        {
        }

        public abstract object ValueAsObject { get; }
    }

    /// <summary>
    /// Base class of a TOML value (bool, string, integer, float, datetime)
    /// </summary>
    public abstract class TomlValue<T> : TomlValue, IEquatable<TomlValue<T>> where T : IEquatable<T>
    {
        private T _value;

        internal TomlValue(ObjectKind kind, T value) : base(kind)
        {
            _value = value;
        }

        public override object ValueAsObject => _value;

        public T Value
        {
            get => _value;
            set => _value = value;
        }

        public bool Equals(TomlValue<T> other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;
            return _value.Equals(other._value);
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (obj.GetType() != this.GetType()) return false;
            return Equals((TomlValue<T>)obj);
        }

        public override int GetHashCode()
        {
            return _value.GetHashCode();
        }
    }
}