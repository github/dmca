package awais.instagrabber.activities;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.graphics.Bitmap;
import android.os.Build;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.webkit.CookieManager;
import android.webkit.CookieSyncManager;
import android.webkit.WebChromeClient;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.CompoundButton;
import android.widget.Toast;

import androidx.annotation.Nullable;

import awais.instagrabber.R;
import awais.instagrabber.databinding.ActivityLoginBinding;
import awais.instagrabber.utils.Constants;
import awais.instagrabber.utils.CookieUtils;
import awais.instagrabber.utils.TextUtils;

public final class Login extends BaseLanguageActivity implements View.OnClickListener {
    private final WebViewClient webViewClient = new WebViewClient() {
        @Override
        public void onPageStarted(final WebView view, final String url, final Bitmap favicon) {
            webViewUrl = url;
        }

        @Override
        public void onPageFinished(final WebView view, final String url) {
            webViewUrl = url;
            final String mainCookie = CookieUtils.getCookie(url);
            if (TextUtils.isEmpty(mainCookie) || !mainCookie.contains("; ds_user_id=")) {
                ready = true;
                return;
            }
            if (mainCookie.contains("; ds_user_id=") && ready) {
                returnCookieResult(mainCookie);
            }
        }
    };

    private void returnCookieResult(final String mainCookie) {
        final Intent intent = new Intent();
        intent.putExtra("cookie", mainCookie);
        setResult(Constants.LOGIN_RESULT_CODE, intent);
        finish();
    }

    private final WebChromeClient webChromeClient = new WebChromeClient();
    private String webViewUrl;
    private boolean ready = false;
    private ActivityLoginBinding loginBinding;

    @Override
    protected void onCreate(@Nullable final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        loginBinding = ActivityLoginBinding.inflate(LayoutInflater.from(getApplicationContext()));
        setContentView(loginBinding.getRoot());

        initWebView();

        loginBinding.cookies.setOnClickListener(this);
        loginBinding.refresh.setOnClickListener(this);
    }

    @Override
    public void onClick(final View v) {
        if (v == loginBinding.refresh) {
            loginBinding.webView.loadUrl("https://instagram.com/");
            return;
        }
        if (v == loginBinding.cookies) {
            final String mainCookie = CookieUtils.getCookie(webViewUrl);
            if (TextUtils.isEmpty(mainCookie) || !mainCookie.contains("; ds_user_id=")) {
                Toast.makeText(this, R.string.login_error_loading_cookies, Toast.LENGTH_SHORT).show();
                return;
            }
            returnCookieResult(mainCookie);
        }
    }

    @SuppressLint("SetJavaScriptEnabled")
    private void initWebView() {
        if (loginBinding != null) {
            loginBinding.webView.setWebChromeClient(webChromeClient);
            loginBinding.webView.setWebViewClient(webViewClient);
            final WebSettings webSettings = loginBinding.webView.getSettings();
            if (webSettings != null) {
                webSettings.setUserAgentString("Mozilla/5.0 (Linux; Android 10) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/89.0.4389.105 Mobile Safari/537.36");
                webSettings.setJavaScriptEnabled(true);
                webSettings.setDomStorageEnabled(true);
                webSettings.setSupportZoom(true);
                webSettings.setBuiltInZoomControls(true);
                webSettings.setDisplayZoomControls(false);
                webSettings.setLoadWithOverviewMode(true);
                webSettings.setUseWideViewPort(true);
                webSettings.setAllowFileAccessFromFileURLs(true);
                webSettings.setAllowUniversalAccessFromFileURLs(true);
                webSettings.setMixedContentMode(WebSettings.MIXED_CONTENT_ALWAYS_ALLOW);
            }

            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP_MR1) {
                CookieManager.getInstance().removeAllCookies(null);
                CookieManager.getInstance().flush();
            } else {
                CookieSyncManager cookieSyncMngr = CookieSyncManager.createInstance(getApplicationContext());
                cookieSyncMngr.startSync();
                CookieManager cookieManager = CookieManager.getInstance();
                cookieManager.removeAllCookie();
                cookieManager.removeSessionCookie();
                cookieSyncMngr.stopSync();
                cookieSyncMngr.sync();
            }
            loginBinding.webView.loadUrl("https://instagram.com/");
        }
    }

    @Override
    protected void onPause() {
        if (loginBinding != null) loginBinding.webView.onPause();
        super.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (loginBinding != null) loginBinding.webView.onResume();
    }

    @Override
    protected void onDestroy() {
        if (loginBinding != null) loginBinding.webView.destroy();
        super.onDestroy();
    }
}