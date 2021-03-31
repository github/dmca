// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
namespace MelonLoader.Tomlyn.Model
{
    /// <summary>
    /// Kind of an TOML object.
    /// </summary>
    public enum ObjectKind
    {
        Table,

        TableArray,

        Array,

        Boolean,

        String,

        Integer,

        Float,

        OffsetDateTime,

        LocalDateTime,

        LocalDate,

        LocalTime,
    }
}