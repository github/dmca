@protocol BBDataProvider;

@interface BBLocalDataProviderStore : NSObject

- (void)addDataProvider:(id <BBDataProvider>)provider performMigration:(BOOL)performMigration;
- (void)addDataProvider:(id <BBDataProvider>)provider;

- (void)_addDataProviderClass:(Class)providerClass performMigration:(BOOL)performMigration;

@end
