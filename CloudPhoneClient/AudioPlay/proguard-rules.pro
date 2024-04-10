# 不混淆某个类（使用者可以看到类名）
-keep class com.huawei.cloudgame.audioplay.AudioTrackPlayer {
    public *;
}
-keep class com.huawei.cloudgame.audioplay.AUDIOSENDHOOK { *; }
# --------------------------基本混淆规则----------------------------------
# 指定代码的压缩级别，默认是5
-optimizationpasses 5
# 指定混淆采用的算法，后面的参数是一个过滤器，这个过滤器是谷歌推荐的算法，一般不做更改
-optimizations !code/simplification/arithmetic,!field/*,!class/merging/*
# 这项配置是设置是否允许改变作用域的。使用这项配置之后可以提高优化的效果。
# 但是，如果你的代码是一个库的话，最好不要配置这个选项，因为它可能会导致一些private变量被改成public。
-allowaccessmodification
# 不进行预校验,Android不需要,可加快混淆速度。
-dontpreverify
# 不使用大小写混合类名，注意，windows用户必须为ProGuard指定该选项，因为windows对文件的大小写是不敏感的，
# 也就是比如a.java和A.java会认为是同一个文件。如果不这样做并且你的项目中有超过26个类的话，那么ProGuard就
# 会默认混用大小写文件名，导致class文件相互覆盖。
-dontusemixedcaseclassnames
# 指定不去忽略非公共的库类，一般情况下非public在应用内是引用不到的，跳过它们也没什么关系。但是，在一些java类
# 库中出现了public类继承非public类的情况，这样就不能用这个选项了。这种情况下，会打印一个警告出来，提示找不到类。
-dontskipnonpubliclibraryclasses
-dontskipnonpubliclibraryclassmembers
# 声明在处理过程中输出更多信息。添加这项配置之后，如果处理过程中出现异常，会输出整个StackTrace而不是一条简单的异常说明。
-verbose
# 避免混淆异常、注解、泛型、内部类、匿名类
-keepattributes Exceptions,*Annotation*,Signature,InnerClasses,EnclosingMethod
# 抛出异常时保留代码行号
-keepattributes SourceFile,LineNumberTable
# 避免混淆已声明的class,filed,或者方法  参数(java8或者以上)
-keepattributes Deprecated,MethodParameters
# 表示不混淆这两个类的类，这两个类我们基本也用不上，是接入Google原生的一些服务时使用的
-keep public class com.google.vending.licensing.ILicensingService
-keep public class com.android.vending.licensing.ILicensingService
# 表示不混淆任何包含native方法的类的类名以及native方法名
-keepclasseswithmembernames class * {
    native <methods>;
}
# 表示不混淆任何一个View中的set和get方法，
-keep public class * extends android.view.View{
    *** get*();
    void set*(***);
}
# 表示不混淆Activity中参数是View的方法，因为有这样一种用法，在XML中配置android:onClick=”buttonClick”属性，
# 当用户点击该按钮时就会调用Activity中的buttonClick(View view)方法，如果这个方法被混淆的话就找不到了
-keepclassmembers class * extends android.app.Activity{
    public void *(android.view.View);
}
# 表示不混淆枚举中的values()和valueOf()方法
-keepclassmembers enum * {
    public static **[] values();
    public static ** valueOf(java.lang.String);
}
# 表示不混淆资源类下static的字段
-keepclassmembers class **.R$* {
    public static <fields>;
}
# 忽略support相关警告
-dontwarn android.support.**
# support-v4包
-keep class android.support.v4.app.** { *; }
-keep interface android.support.v4.app.** { *; }
-keep class android.support.v4.** { *; }
# support-v7包
-keep class android.support.v7.internal.** { *; }
-keep interface android.support.v7.internal.** { *; }
-keep class android.support.v7.** { *; }
# support design
-keep class android.support.design.** { *; }
-keep interface android.support.design.** { *; }
-keep public class android.support.design.R$* { *; }
-keep class android.support.annotation.Keep
# 保持被Keep注解修饰的类、方法、变量、构造函数不混淆
-keep @android.support.annotation.Keep class * {*;}
-keepclasseswithmembers class * {
    @android.support.annotation.Keep <methods>;
}
-keepclasseswithmembers class * {
    @android.support.annotation.Keep <fields>;
}
-keepclasseswithmembers class * {
    @android.support.annotation.Keep <init>(...);
}
# 表示不混淆Parcelable类中的CREATOR字段，
-keep class * implements android.os.Parcelable {
    public static final android.os.Parcelable$Creator *;
}
# 这指定了继承Serizalizable的类的如下成员不被移除混淆
-keepclassmembers class * implements java.io.Serializable {
    static final long serialVersionUID;
    private static final java.io.ObjectStreamField[] serialPersistentFields;
    private void writeObject(java.io.ObjectOutputStream);
    private void readObject(java.io.ObjectInputStream);
    java.lang.Object writeReplace();
    java.lang.Object readResolve();
}
# 删除代码中Log相关的代码
# -assumenosideeffects class android.util.Log {
#     public static boolean isLoggable(java.lang.String, int);
#     public static int v(...);
#     public static int i(...);
#     public static int w(...);
#     public static int d(...);
#     public static int e(...);
# }
