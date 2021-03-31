// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
using System;
using System.Globalization;

namespace MelonLoader.Tomlyn.Model
{
    /// <summary>
    /// Runtime representation of a TOML datetime
    /// </summary>
    public sealed class TomlDateTime : TomlValue<DateTime>
    {
        public TomlDateTime(ObjectKind kind, DateTime value) : base(CheckDateTimeKind(kind), value)
        {
        }

        private static ObjectKind CheckDateTimeKind(ObjectKind kind)
        {
            switch (kind)
            {
                case ObjectKind.OffsetDateTime:
                case ObjectKind.LocalDateTime:
                case ObjectKind.LocalDate:
                case ObjectKind.LocalTime:
                    return kind;
                default:
                    throw new ArgumentOutOfRangeException(nameof(kind), kind, null);
            }
        }

        public override string ToString()
        {
            switch (Kind)
            {
                case ObjectKind.OffsetDateTime:
                    var time = Value.ToUniversalTime();
                    if (time.Millisecond == 0) return time.ToString("yyyy-MM-dd'T'HH:mm:ssK", CultureInfo.InvariantCulture);
                    return time.ToString("yyyy-MM-dd'T'HH:mm:ss.fffK", CultureInfo.InvariantCulture);
                case ObjectKind.LocalDateTime:
                    if (Value.Millisecond == 0) return Value.ToString("yyyy-MM-dd'T'HH:mm:ss", CultureInfo.InvariantCulture);
                    return Value.ToString("yyyy-MM-dd'T'HH:mm:ss.fff", CultureInfo.InvariantCulture);
                case ObjectKind.LocalDate:
                    return Value.ToString("yyyy-MM-dd", CultureInfo.InvariantCulture);
                case ObjectKind.LocalTime:
                    return Value.Millisecond == 0 ? Value.ToString("HH:mm:ss", CultureInfo.InvariantCulture) : Value.ToString("HH:mm:ss.fff", CultureInfo.InvariantCulture);
            }
            // Return empty, should never happen as the constructor is protecting us
            return string.Empty;
        }
    }
}