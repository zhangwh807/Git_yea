/***************************Accessing Instance Field*************************/
JNIEXPORT void JNICALL
Java_InstanceFieldAccess_accessField(JNIEnv *env, jobject obj)
{
	jfieldID fid; /* store the field ID */
	jstring jstr;
	const char *str;
	
	/* Get a reference to obj’s class */
	jclass cls = (*env)->GetObjectClass(env, obj);
	printf("In C:\n");
	
	/* Look for the instance field s in cls */
	fid = (*env)->GetFieldID(env, cls, "s", "Ljava/lang/String;");
	if (fid == NULL) {
		return; /* failed to find the field */
	}
	
	/* Read the instance field s */
	jstr = (*env)->GetObjectField(env, obj, fid);
	str = (*env)->GetStringUTFChars(env, jstr, NULL);
	if (str == NULL) {
		return; /* out of memory */
	}
	
	printf(" c.s = \"%s\"\n", str);
	(*env)->ReleaseStringUTFChars(env, jstr, str);
	
	/* Create a new string and overwrite the instance field */
	jstr = (*env)->NewStringUTF(env, "123");
	if (jstr == NULL) {
		return; /* out of memory */
	}
	(*env)->SetObjectField(env, obj, fid, jstr);
}

/***************************Accessing Static Field*************************/
JNIEXPORT void JNICALL
Java_StaticFieldAccess_accessField(JNIEnv *env, jobject obj)
{
	jfieldID fid; /* store the field ID */
	jint si;
	
	/* Get a reference to obj’s class */
	jclass cls = (*env)->GetObjectClass(env, obj);
	printf("In C:\n");
	
	/* Look for the static field si in cls */
	fid = (*env)->GetStaticFieldID(env, cls, "si", "I");
	if (fid == NULL) {
		return; /* field not found */
	}
	
	/* Access the static field si */
	si = (*env)->GetStaticIntField(env, cls, fid);
	printf(" StaticFieldAccess.si = %d\n", si);
	(*env)->SetStaticIntField(env, cls, fid, 200);
}

/***************************Calling Instance Method*************************/
JNIEXPORT void JNICALL
Java_InstanceMethodCall_nativeMethod(JNIEnv *env, jobject obj)
{
	jclass cls = (*env)->GetObjectClass(env, obj);
	jmethodID mid = (*env)->GetMethodID(env, cls, "callback", "()V");
	if (mid == NULL) {
		return; /* method not found */
	}
	
	printf("In C\n");
	(*env)->CallVoidMethod(env, obj, mid);
}

/***************************Calling Static Method*************************/
1. static方法形参(JNIEnv *env, jclass cls,...)
1. jmethodID mid = (*env)->GetStaticMethodID(env, cls, "MethodName", "Signiture");
2. (*env)->CallStatic<ReturnValueType>Method(env, cls, mid, ?parameter);

/***************************Exceptions in Utility Functions*************************/
jvalue
JNU_CallMethodByName(JNIEnv *env,
					 jboolean *hasException,
					 jobject obj,
					 const char *name,
					 const char *descriptor,
					 ...)
{
	va_list args;
	jclass clazz;
	jmethodID mid;
	jvalue result;
	if ((*env)->EnsureLocalCapacity(env, 2) == JNI_OK) {
		clazz = (*env)->GetObjectClass(env, obj);
		mid = (*env)->GetMethodID(env, clazz, name, descriptor);
		if (mid) {
			const char *p = descriptor;
			/* skip over argument types to find out the
			return type */
			while (*p != ')') p++;
			/* skip ')' */
			p++;
			va_start(args, descriptor);
			switch (*p) {
				case 'V':
					(*env)->CallVoidMethodV(env, obj, mid, args);
					break;
				case '[':
				case 'L':
					result.l = (*env)->CallObjectMethodV(env, obj, mid, args);
					break;
				case 'Z':
					result.z = (*env)->CallBooleanMethodV(env, obj, mid, args);
					break;
				case 'B':
					result.b = (*env)->CallByteMethodV(env, obj, mid, args);
					break;
				case 'C':
					result.c = (*env)->CallCharMethodV(env, obj, mid, args);
					break;
				case 'S':
					result.s = (*env)->CallShortMethodV(env, obj, mid, args);
					break;
				case 'I':
					result.i = (*env)->CallIntMethodV(env, obj, mid, args);
					break;
				case 'J':
					result.j = (*env)->CallLongMethodV(env, obj, mid, args);
					break;
				case 'F':
					result.f = (*env)->CallFloatMethodV(env, obj, mid, args);
					break;
				case 'D':
					result.d = (*env)->CallDoubleMethodV(env, obj, mid, args);
					break;
				default:
					(*env)->FatalError(env, "illegal descriptor");
			}
			va_end(args);
		}
		(*env)->DeleteLocalRef(env, clazz);
	}
	if (hasException) {
		*hasException = (*env)->ExceptionCheck(env);
	}
	return result;
}

/***************************Creating the Java Virtual Machcine*************************/
public class Prog {
	public static void main(String[] args) {
		System.out.println("Hello World " + args[0]);
	}
}
/*************************************C代码*************************************/
#include <jni.h>
#define PATH_SEPARATOR ';' /* define it to be ':' on Solaris */
#define USER_CLASSPATH "." /* where Prog.class is */

main() {
	JNIEnv *env;
	JavaVM *jvm;
	jint res;
	jclass cls;
	jmethodID mid;
	jstring jstr;
	jclass stringClass;
	jobjectArray args;
	
	#ifdef JNI_VERSION_1_2
		JavaVMInitArgs vm_args;
		JavaVMOption options[1];
		options[0].optionString =
		"-Djava.class.path=" USER_CLASSPATH;
		vm_args.version = 0x00010002;
		vm_args.options = options;
		vm_args.nOptions = 1;
		vm_args.ignoreUnrecognized = JNI_TRUE;
		/* Create the Java VM */
		res = JNI_CreateJavaVM(&jvm, (void**)&env, &vm_args);
	#else
		JDK1_1InitArgs vm_args;
		char classpath[1024];
		vm_args.version = 0x00010001;
		JNI_GetDefaultJavaVMInitArgs(&vm_args);
		/* Append USER_CLASSPATH to the default system class path */
		sprintf(classpath, "%s%c%s",
		vm_args.classpath, PATH_SEPARATOR, USER_CLASSPATH);
		vm_args.classpath = classpath;
		/* Create the Java VM */
		res = JNI_CreateJavaVM(&jvm, &env, &vm_args);
	#endif /* JNI_VERSION_1_2 */
	
	if (res < 0) {
		fprintf(stderr, "Can't create Java VM\n");
		exit(1);
	}
	
	cls = (*env)->FindClass(env, "Prog");
	if (cls == NULL) {
		goto destroy;
	}
	
	mid = (*env)->GetStaticMethodID(env, cls, "main", "([Ljava/lang/String;)V");
	if (mid == NULL) {
		goto destroy;
	}
	
	jstr = (*env)->NewStringUTF(env, " from C!");
	if (jstr == NULL) {
		goto destroy;
	}
	
	stringClass = (*env)->FindClass(env, "java/lang/String");
	args = (*env)->NewObjectArray(env, 1, stringClass, jstr);
	if (args == NULL) {
		goto destroy;
	}
	
	(*env)->CallStaticVoidMethod(env, cls, mid, args);
	
	destroy:
	if ((*env)->ExceptionOccurred(env)) {
		(*env)->ExceptionDescribe(env);
	}
	(*jvm)->DestroyJavaVM(jvm);
}



