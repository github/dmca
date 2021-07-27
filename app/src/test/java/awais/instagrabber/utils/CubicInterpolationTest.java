package awais.instagrabber.utils;

import org.junit.jupiter.api.Test;

class CubicInterpolationTest {

    @Test
    void interpolate() {
        final float[] array = {0,
                0,
                0,
                0,
                0.020099867f,
                0.26693913f,
                0.29802227f,
                0.41319302f,
                0.63605154f,
                0.55332285f,
                0.9193651f,
                0.9632098f,
                0.538488f,
                0.4350556f,
                0.18891974f,
                0.89930296f,
                0.89224446f,
                0.31689966f,
                0.10953838f,
                0.12687835f,
                0.10990722f,
                1.4566885f,
                1.7016107f,
                0.9643246f,
                0.48731846f,
                1.0107778f,
                0.7347803f,
                0.5262502f,
                0.54368f,
                0.44617367f,
                0.14939539f,
                1.214883f,
                1.6565621f,
                2.1008852f,
                1.4302688f,
                2.00104f,
                0.62863034f,
                0.10548139f,
                0.12392626f,
                0.09700036f,
                0.22292629f,
                1.9768263f,
                1.0130078f,
                0.14681084f,
                0.116178185f,
                1.8114564f,
                2.68573f,
                2.4381645f};
        final CubicInterpolation cubicInterpolation = new CubicInterpolation(array);
        final float interpolate = cubicInterpolation.interpolate(5.25f);
        System.out.println(interpolate);
    }
}