TARGET = Undecimus

.PHONY: all clean

all: clean
	xcodebuild clean build -quiet CODE_SIGN_IDENTITY="" CODE_SIGNING_REQUIRED=NO CODE_SIGNING_ALLOWED=NO PRODUCT_BUNDLE_IDENTIFIER="science.xnu.undecimus" -sdk iphoneos -configuration Release
	ln -sf build/Release-iphoneos Payload
	xcrun strip Payload/$(TARGET).app/$(TARGET)
	ldid -SUndecimus/Undecimus.entitlements Payload/$(TARGET).app/$(TARGET)
	zip -r9q $(TARGET).ipa Payload/$(TARGET).app

clean:
	rm -rf build Payload $(TARGET).ipa
