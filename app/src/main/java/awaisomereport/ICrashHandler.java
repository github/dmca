package awaisomereport;

public interface ICrashHandler {
    void uncaughtException(Thread t,
                           Throwable exception,
                           Thread.UncaughtExceptionHandler defaultEH);
}
