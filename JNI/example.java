/***************************Accessing Instance Field*************************/
class InstanceFieldAccess {
	private String s;
	private native void accessField();
	
	public static void main(String args[]) {
		InstanceFieldAccess c = new InstanceFieldAccess();
		c.s = "abc";
		c.accessField();
		System.out.println("In Java:");
		System.out.println(" c.s = \"" + c.s + "\"");
	}
	
	static {
		System.loadLibrary("InstanceFieldAccess");
	}
}
/***************************Accessing Static Field*************************/
class StaticFielcdAccess {
	private static int si;
	private native void accessField();
	
	public static void main(String args[]) {
		StaticFieldAccess c = new StaticFieldAccess();
		StaticFieldAccess.si = 100;
		c.accessField();
		System.out.println("In Java:");
		System.out.println(" StaticFieldAccess.si = " + si);
	}
	
	static {
		System.loadLibrary("StaticFieldAccess");
	}
}

/***************************Calling Instance Method*************************/
class InstanceMethodCall {
	private native void nativeMethod();
	private void callback() {
		System.out.println("In Java");
	}
	
	public static void main(String args[]) {
		InstanceMethodCall c = new InstanceMethodCall();
		c.nativeMethod();
	}
	
	static {
		System.loadLibrary("InstanceMethodCall");
	}
}

/***************************Calling Static Method*************************/
......


