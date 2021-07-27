package awais.instagrabber.webservices;

public abstract class BaseService {
    private static final String TAG = "BaseService";

    // protected String userBreadcrumb(final int size) {
    //     final long term = (random(2, 4) * 1000) + size + (random(15, 21) * 1000);
    //     final float div = (float) size / random(2, 4);
    //     final int round = Math.round(div);
    //     final long textChangeEventCount = round > 0 ? round : 1;
    //     final String data = String.format(Locale.getDefault(), "%d %d %d %d", size, term, textChangeEventCount, new Date().getTime());
    //     try {
    //         final Mac hasher = Mac.getInstance("HmacSHA256");
    //         hasher.init(new SecretKeySpec(Constants.BREADCRUMB_KEY.getBytes(), "HmacSHA256"));
    //         byte[] hash = hasher.doFinal(data.getBytes());
    //         final StringBuilder hexString = new StringBuilder();
    //         for (byte b : hash) {
    //             final String hex = Integer.toHexString(0xff & b);
    //             if (hex.length() == 1) hexString.append('0');
    //             hexString.append(hex);
    //         }
    //         final String encodedData = Base64.encodeToString(data.getBytes(), Base64.NO_WRAP);
    //         final String encodedHex = Base64.encodeToString(hexString.toString().getBytes(), Base64.NO_WRAP);
    //         return String.format("%s\n%s\n", encodedHex, encodedData);
    //     } catch (Exception e) {
    //         Log.e(TAG, "Error creating breadcrumb", e);
    //         return null;
    //     }
    // }
}
