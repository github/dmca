package awais.instagrabber.utils;

import java.util.Arrays;

public class CubicInterpolation {

    private final float[] array;
    private final int tangentFactor;
    private final int length;

    // constructor: (array, config) ->
    // 		@array = array.slice 0 #copy the array
    // 		@length = @array.length #cache length

    // class CubicInterpolator extends AbstractInterpolator
    // 	constructor: (array, config)->
    // 		#clamp cubic tension to [0,1] range
    // 		@tangentFactor = 1 - Math.max 0, Math.min 1, config.cubicTension
    // 		super
    //
    // 	# Cardinal spline with tension 0.5)
    // 	getTangent: (k) -> @tangentFactor*(@getClippedInput(k + 1) - @getClippedInput(k - 1))/2
    //
    // 	interpolate: (t) ->
    // 		k = Math.floor t
    // 		m = [(@getTangent k), (@getTangent k+1)] #get tangents
    // 		p = [(@getClippedInput k), (@getClippedInput k+1)] #get points
    // 		#Translate t to interpolate between k and k+1
    // 		t -= k
    // 		t2 = t*t #t^2
    // 		t3 = t*t2 #t^3
    // 		#Apply cubic hermite spline formula
    // 		return (2*t3 - 3*t2 + 1)*p[0] + (t3 - 2*t2 + t)*m[0] + (-2*t3 + 3*t2)*p[1] + (t3 - t2)*m[1]
    public CubicInterpolation(final float[] array, final int cubicTension) {
        this.array = Arrays.copyOf(array, array.length);
        this.length = array.length;
        tangentFactor = 1 - Math.max(0, Math.min(1, cubicTension));
    }

    public CubicInterpolation(final float[] array) {
        this(array, 0);
    }

    private float getTangent(int k) {
        return tangentFactor * (getClippedInput(k + 1) - getClippedInput(k - 1)) / 2;
    }

    public float interpolate(final float t) {
        int k = (int) Math.floor(t);
        float[] m = new float[]{getTangent(k), getTangent(k + 1)};
        float[] p = new float[]{getClippedInput(k), getClippedInput(k + 1)};
        final float t1 = t - k;
        final float t2 = t1 * t1;
        final float t3 = t1 * t2;
        return (2 * t3 - 3 * t2 + 1) * p[0] + (t3 - 2 * t2 + t1) * m[0] + (-2 * t3 + 3 * t2) * p[1] + (t3 - t2) * m[1];
    }

    // getClippedInput: (i) ->
    // 		#Normal behavior for indexes within bounds
    // 		if 0 <= i < @length
    // 			@array[i]
    // 		else
    // 			@clipHelper i
    //
    // 	clipHelperClamp: (i) -> @array[clipClamp i, @length]
    // clipClamp = (i, n) -> Math.max 0, Math.min i, n - 1

    private float getClippedInput(int i) {
        if (i >= 0 && i < length) {
            return array[i];
        }
        return array[clipClamp(i, length)];
    }

    private int clipClamp(int i, int n) {
        return Math.max(0, Math.min(i, n - 1));
    }
}
