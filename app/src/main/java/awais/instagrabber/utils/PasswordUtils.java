package awais.instagrabber.utils;

import android.util.Base64;

import androidx.annotation.NonNull;

import java.security.GeneralSecurityException;
import java.security.InvalidAlgorithmParameterException;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;

import javax.crypto.BadPaddingException;
import javax.crypto.Cipher;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.NoSuchPaddingException;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;

public final class PasswordUtils {
    private static final String cipherAlgo = "AES";
    private static final String cipherTran = "AES/CBC/PKCS5Padding";

    public static byte[] dec(final String encrypted, final byte[] keyValue) throws Exception {
        try {
            final Cipher cipher = Cipher.getInstance(cipherTran);
            final SecretKeySpec secretKey = new SecretKeySpec(keyValue, cipherAlgo);
            cipher.init(Cipher.DECRYPT_MODE, secretKey, new IvParameterSpec(new byte[16]));
            return cipher.doFinal(Base64.decode(encrypted, Base64.DEFAULT | Base64.NO_PADDING | Base64.NO_WRAP));
        } catch (NoSuchAlgorithmException | NoSuchPaddingException | InvalidAlgorithmParameterException | InvalidKeyException | BadPaddingException | IllegalBlockSizeException e) {
            throw new IncorrectPasswordException(e);
        }
    }

    public static byte[] enc(@NonNull final String str, final byte[] keyValue) throws Exception {
        final Cipher cipher = Cipher.getInstance(cipherTran);
        final SecretKeySpec secretKey = new SecretKeySpec(keyValue, cipherAlgo);
        cipher.init(Cipher.ENCRYPT_MODE, secretKey, new IvParameterSpec(new byte[16]));
        final byte[] bytes = cipher.doFinal(str.getBytes());
        return Base64.encode(bytes, Base64.DEFAULT | Base64.NO_PADDING | Base64.NO_WRAP);
    }

    public static class IncorrectPasswordException extends Exception {
        public IncorrectPasswordException(final GeneralSecurityException e) {
            super(e);
        }
    }
}
