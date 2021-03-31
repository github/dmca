// Copyright (c) Alexandre Mutel. All rights reserved.
// Licensed under the BSD-Clause 2 license. 
// See license.txt file in the project root for full license information.
using System;
using System.Globalization;

namespace MelonLoader.Tomlyn.Helpers
{
    internal static class DateTimeRFC3339
    {
        // https://www.ietf.org/rfc/rfc3339.txt

        //date-fullyear   = 4DIGIT
        //date-month      = 2DIGIT  ; 01-12
        //date-mday       = 2DIGIT  ; 01-28, 01-29, 01-30, 01-31 based on
        //                          ; month/year
        //time-hour       = 2DIGIT  ; 00-23
        //time-minute     = 2DIGIT  ; 00-59
        //time-second     = 2DIGIT  ; 00-58, 00-59, 00-60 based on leap second
        //                          ; rules
        //time-secfrac    = "." 1*DIGIT
        //time-numoffset  = ("+" / "-") time-hour ":" time-minute
        //time-offset     = "Z" / time-numoffset

        //partial-time    = time-hour ":" time-minute ":" time-second
        //                  [time-secfrac]
        //full-date       = date-fullyear "-" date-month "-" date-mday
        //full-time       = partial-time time-offset

        //date-time       = full-date "T" full-time

        private static readonly string[] OffsetDateTimeFormats = new[]
        {            
            "yyyy-MM-ddTHH:mm:ssZ",            // With Z postfix
            "yyyy-MM-ddTHH:mm:ss.fZ",
            "yyyy-MM-ddTHH:mm:ss.ffZ",
            "yyyy-MM-ddTHH:mm:ss.fffZ",
            "yyyy-MM-ddTHH:mm:ss.ffffZ",
            "yyyy-MM-ddTHH:mm:ss.fffffZ",
            "yyyy-MM-ddTHH:mm:ss.ffffffZ",
            "yyyy-MM-ddTHH:mm:ss.fffffffZ",
            "yyyy-MM-ddTHH:mm:sszzz",          // With time-numoffset
            "yyyy-MM-ddTHH:mm:ss.fzzz",
            "yyyy-MM-ddTHH:mm:ss.ffzzz",
            "yyyy-MM-ddTHH:mm:ss.fffzzz",
            "yyyy-MM-ddTHH:mm:ss.ffffzzz",
            "yyyy-MM-ddTHH:mm:ss.fffffzzz",
            "yyyy-MM-ddTHH:mm:ss.ffffffzzz",
            "yyyy-MM-ddTHH:mm:ss.fffffffzzz",

            // Specs says that T might be omitted
            "yyyy-MM-dd HH:mm:ssZ",            // With Z postfix
            "yyyy-MM-dd HH:mm:ss.fZ",
            "yyyy-MM-dd HH:mm:ss.ffZ",
            "yyyy-MM-dd HH:mm:ss.fffZ",
            "yyyy-MM-dd HH:mm:ss.ffffZ",
            "yyyy-MM-dd HH:mm:ss.fffffZ",
            "yyyy-MM-dd HH:mm:ss.ffffffZ",
            "yyyy-MM-dd HH:mm:ss.fffffffZ",
            "yyyy-MM-dd HH:mm:sszzz",          // With time-numoffset
            "yyyy-MM-dd HH:mm:ss.fzzz",
            "yyyy-MM-dd HH:mm:ss.ffzzz",
            "yyyy-MM-dd HH:mm:ss.fffzzz",
            "yyyy-MM-dd HH:mm:ss.ffffzzz",
            "yyyy-MM-dd HH:mm:ss.fffffzzz",
            "yyyy-MM-dd HH:mm:ss.ffffffzzz",
            "yyyy-MM-dd HH:mm:ss.fffffffzzz",
        };

        private static readonly string[] LocalDateTimeFormats = new[]
        {
            "yyyy-MM-ddTHH:mm:ss",
            "yyyy-MM-ddTHH:mm:ss.f",
            "yyyy-MM-ddTHH:mm:ss.ff",
            "yyyy-MM-ddTHH:mm:ss.fff",
            "yyyy-MM-ddTHH:mm:ss.ffff",
            "yyyy-MM-ddTHH:mm:ss.fffff",
            "yyyy-MM-ddTHH:mm:ss.ffffff",
            "yyyy-MM-ddTHH:mm:ss.fffffff",

            // Specs says that T might be omitted
            "yyyy-MM-dd HH:mm:ss",            // With Z postfix
            "yyyy-MM-dd HH:mm:ss.f",
            "yyyy-MM-dd HH:mm:ss.ff",
            "yyyy-MM-dd HH:mm:ss.fff",
            "yyyy-MM-dd HH:mm:ss.ffff",
            "yyyy-MM-dd HH:mm:ss.fffff",
            "yyyy-MM-dd HH:mm:ss.ffffff",
            "yyyy-MM-dd HH:mm:ss.fffffff",
        };

        // Local Time
        private static readonly string[] LocalTimeFormats = new[]
        {
            "HH:mm:ss",
            "HH:mm:ss.f",
            "HH:mm:ss.ff",
            "HH:mm:ss.fff",
            "HH:mm:ss.ffff",
            "HH:mm:ss.fffff",
            "HH:mm:ss.ffffff",
            "HH:mm:ss.fffffff",
        };

        public static bool TryParseOffsetDateTime(string str, out DateTime time)
        {
            return DateTime.TryParseExact(str, OffsetDateTimeFormats, CultureInfo.InvariantCulture, DateTimeStyles.None, out time);
        }

        public static bool TryParseLocalDateTime(string str, out DateTime time)
        {
            return DateTime.TryParseExact(str, LocalDateTimeFormats, CultureInfo.InvariantCulture, DateTimeStyles.None, out time);
        }

        public static bool TryParseLocalDate(string str, out DateTime time)
        {
            return DateTime.TryParseExact(str, "yyyy-MM-dd", CultureInfo.InvariantCulture, DateTimeStyles.None, out time);
        }

        public static bool TryParseLocalTime(string str, out DateTime time)
        {
            return DateTime.TryParseExact(str, LocalTimeFormats, CultureInfo.InvariantCulture, DateTimeStyles.None, out time);
        }
    }
}