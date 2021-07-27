package awais.instagrabber.utils;

import android.util.Pair;

import java.io.Serializable;

public class SerializablePair<F, S> extends Pair<F, S> implements Serializable {
    /**
     * Constructor for a Pair.
     *
     * @param first  the first object in the Pair
     * @param second the second object in the pair
     */
    public SerializablePair(final F first, final S second) {
        super(first, second);
    }
}
