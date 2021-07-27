package awais.instagrabber.utils;

/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.util.ObjectsCompat;

/**
 * Container to ease passing around a tuple of two objects. This object provides a sensible
 * implementation of equals(), returning true if equals() is true on each of the contained
 * objects.
 */
public class NullSafePair<F, S> {
    public final @NonNull
    F first;
    public final @NonNull
    S second;

    /**
     * Constructor for a Pair.
     *
     * @param first  the first object in the Pair
     * @param second the second object in the pair
     */
    public NullSafePair(@NonNull F first, @NonNull S second) {
        this.first = first;
        this.second = second;
    }

    /**
     * Checks the two objects for equality by delegating to their respective
     * {@link Object#equals(Object)} methods.
     *
     * @param o the {@link androidx.core.util.Pair} to which this one is to be checked for equality
     * @return true if the underlying objects of the Pair are both considered
     * equal
     */
    @Override
    public boolean equals(Object o) {
        if (!(o instanceof androidx.core.util.Pair)) {
            return false;
        }
        androidx.core.util.Pair<?, ?> p = (androidx.core.util.Pair<?, ?>) o;
        return ObjectsCompat.equals(p.first, first) && ObjectsCompat.equals(p.second, second);
    }

    /**
     * Compute a hash code using the hash codes of the underlying objects
     *
     * @return a hashcode of the Pair
     */
    @Override
    public int hashCode() {
        return first.hashCode() ^ second.hashCode();
    }

    @NonNull
    @Override
    public String toString() {
        return "Pair{" + first + " " + second + "}";
    }

    /**
     * Convenience method for creating an appropriately typed pair.
     *
     * @param a the first object in the Pair
     * @param b the second object in the pair
     * @return a Pair that is templatized with the types of a and b
     */
    @NonNull
    public static <A, B> androidx.core.util.Pair<A, B> create(@Nullable A a, @Nullable B b) {
        return new androidx.core.util.Pair<A, B>(a, b);
    }
}

