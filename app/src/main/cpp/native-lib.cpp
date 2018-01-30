#include <jni.h>
#include <string>
#include "md5.h"
#include "android/log.h"

using namespace std;

// md5附加字符串
static char *EXTRA_SIGNAURE = "fastaoe";

// 校验签名
static int is_verify = 0;
static char *PACKAGE_NAME = "com.fastaoe.ndkdemo1";
static char *APP_SIGNATURE = "3082030d308201f5a0030201020204332314ee300d06092a864886f70d01010b05003037310b30090603550406130255533110300e060355040a1307416e64726f6964311630140603550403130d416e64726f6964204465627567301e170d3136303130343034353533305a170d3435313232373034353533305a3037310b30090603550406130255533110300e060355040a1307416e64726f6964311630140603550403130d416e64726f696420446562756730820122300d06092a864886f70d01010105000382010f003082010a0282010100aa994ce0e32a5ecdaf56c8b3b54b7f812461c63712bbc76fd1ceaec2a4e241caef2deb6ac57332534859ff74b57c74689ac6f9108e2f02ec9c508a45d950b7ca6af9794b61220e8abf877d33dfd845b33bfc51a7907a41fe2179f797c4cb33369f8661a44b0b0beaf532f83b750bec4549f2226717e283afe4f90f3a1c772f0c277ebe5c598b5f67f4fe1b0420a118b5d6480e15e55a29ef5d3201868c91bc99aa7c6efddec528c4adedde44cf7caed4609a7e22344db15ff73a237715d780b7f6941dbf27227737dfda6a897af826b0a9786a2dd7694f2c456cc1a6b00297c54e01bd6b7a369a87d69662bce1eabb0d359da48f167896543eff28a17559c1170203010001a321301f301d0603551d0e041604142659f515af63b0b0cc2f696cae7bc145b2e65641300d06092a864886f70d01010b050003820101005bcc4519b72ac55a3c6ba2b7d6e5048aa270b7c0f243427b9eaebbe9e1b5b97ae3f1e301a12d7a06d5902d96528da0287f36ec2326cfb4a321172922303df68263a2f1a021675f6d068654912de17fb32b924de4f1e2fce5efdff33e5d04d71770c1a8931643fe4c7d541629f419e32176be13e05eee9af3c58b803a2d5c6c13dd8dc2e4af1b0e5fb69229ab41738fcb8a61df7258910d56f7312c1f707eae4ee1fd0a165233350a0962a7a7afef49ba1cae4b4b007660330a18aa77e3ff7251f21e9e75f10b09775eef6fe38655b9094b0bbc4b535cb2290dce83b4bc700d24c4235beeec995c48bc94a89e307ba4b59be3107b91c25a63231a0c463a52f839";


extern "C" {
JNIEXPORT jstring JNICALL
Java_com_fastaoe_ndkdemo1_SignatureUtil_signatureParams(JNIEnv *env, jclass type, jstring text_);

JNIEXPORT void JNICALL
Java_com_fastaoe_ndkdemo1_SignatureUtil_signatureVerify(JNIEnv *env, jclass type, jobject context);
}

JNIEXPORT jstring JNICALL
Java_com_fastaoe_ndkdemo1_SignatureUtil_signatureParams(JNIEnv *env, jclass type, jstring text_) {

    if (is_verify == 0) { return env->NewStringUTF("error_signature!"); }

    const char *text = env->GetStringUTFChars(text_, 0);

    // 1. 将字符串修改（一定规则）
    string signature_params(text);
    // 添加头
    signature_params.insert(0, EXTRA_SIGNAURE);
    // 去掉末尾2位
    signature_params = signature_params.substr(0, signature_params.length() - 2);

    // 2. 使用md5签名
    MD5_CTX *ctx = new MD5_CTX();
    MD5Init(ctx);
    MD5Update(ctx, (unsigned char *) signature_params.c_str(), signature_params.length());
    unsigned char digest[16] = {0};
    MD5Final(digest, ctx);

    // 3. 生产32位字符串
    char md5_str[32];
    for (int i = 0; i < 16; ++i) {
        sprintf(md5_str, "%s%02x", md5_str, digest[i]);
    }

    env->ReleaseStringUTFChars(text_, text);

    return env->NewStringUTF(md5_str);
}

JNIEXPORT void JNICALL
Java_com_fastaoe_ndkdemo1_SignatureUtil_signatureVerify(JNIEnv *env, jclass type, jobject context) {

//    PackageInfo packageInfo = getPackageManager().getPackageInfo(getPackageName(), PackageManager.GET_SIGNATURES);
//    packageInfo.signatures[0].toCharsString();

    // 1. 获取包名
    jclass j_clz = env->GetObjectClass(context);
    jmethodID j_mid = env->GetMethodID(j_clz, "getPackageName", "()Ljava/lang/String;");
    jstring j_package_name = (jstring) env->CallObjectMethod(context, j_mid);
    // 2. 比较
    char *c_package_name = (char *) env->GetStringUTFChars(j_package_name, NULL);
    if (strcmp(c_package_name, PACKAGE_NAME) != 0) {
        return;
    }

    __android_log_print(ANDROID_LOG_ERROR, "JNI_TAG", "包名一致！: %s", PACKAGE_NAME);

    // 3. 获取签名
    // 获取PackageManager
    j_mid = env->GetMethodID(j_clz, "getPackageManager", "()Landroid/content/pm/PackageManager;");
    jobject j_package_manager = env->CallObjectMethod(context, j_mid);
    // 获取packageinfo
    j_clz = env->GetObjectClass(j_package_manager);
    j_mid = env->GetMethodID(j_clz, "getPackageInfo",
                             "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");
    jobject j_package_info = env->CallObjectMethod(j_package_manager,
                                                   j_mid,
                                                   j_package_name,
                                                   0x00000040);
    // 获取signature数组
    j_clz = env->GetObjectClass(j_package_info);
    jfieldID j_fid = env->GetFieldID(j_clz, "signatures", "[Landroid/content/pm/Signature;");
    jobjectArray j_signature = (jobjectArray) env->GetObjectField(j_package_info, j_fid);
    // 获取signatures[0]
    jobject signatures_first = env->GetObjectArrayElement(j_signature, 0);
    // signatures[0].toCharsString()
    j_clz = env->GetObjectClass(signatures_first);
    j_mid = env->GetMethodID(j_clz,"toCharsString","()Ljava/lang/String;");
    jstring j_signature_str = (jstring) env->CallObjectMethod(signatures_first, j_mid);
    // 4. 比较
    const char *c_signature_str = env->GetStringUTFChars(j_signature_str, NULL);
    if (strcmp(c_signature_str, APP_SIGNATURE) != 0) {
        return;
    }

    __android_log_print(ANDROID_LOG_ERROR,"JNI_TAG","签名校验成功: %s",c_signature_str);

    is_verify = 1;
}