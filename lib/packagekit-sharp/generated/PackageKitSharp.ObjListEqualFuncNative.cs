// This file was generated by the Gtk# code generator.
// Any changes made will be lost if regenerated.

namespace PackageKitSharp {

	using System;
	using System.Runtime.InteropServices;

#region Autogenerated code
	[GLib.CDeclCallback]
	internal delegate bool ObjListEqualFuncNative(IntPtr data1, IntPtr data2);

	internal class ObjListEqualFuncInvoker {

		ObjListEqualFuncNative native_cb;
		IntPtr __data;
		GLib.DestroyNotify __notify;

		~ObjListEqualFuncInvoker ()
		{
			if (__notify == null)
				return;
			__notify (__data);
		}

		internal ObjListEqualFuncInvoker (ObjListEqualFuncNative native_cb) : this (native_cb, IntPtr.Zero, null) {}

		internal ObjListEqualFuncInvoker (ObjListEqualFuncNative native_cb, IntPtr data) : this (native_cb, data, null) {}

		internal ObjListEqualFuncInvoker (ObjListEqualFuncNative native_cb, IntPtr data, GLib.DestroyNotify notify)
		{
			this.native_cb = native_cb;
			__data = data;
			__notify = notify;
		}

		internal PackageKit.ObjListEqualFunc Handler {
			get {
				return new PackageKit.ObjListEqualFunc(InvokeNative);
			}
		}

		bool InvokeNative (IntPtr data1, IntPtr data2)
		{
			bool result = native_cb (data1, data2);
			return result;
		}
	}

	internal class ObjListEqualFuncWrapper {

		public bool NativeCallback (IntPtr data1, IntPtr data2)
		{
			try {
				bool __ret = managed (data1, data2);
				if (release_on_call)
					gch.Free ();
				return __ret;
			} catch (Exception e) {
				GLib.ExceptionManager.RaiseUnhandledException (e, false);
				return false;
			}
		}

		bool release_on_call = false;
		GCHandle gch;

		public void PersistUntilCalled ()
		{
			release_on_call = true;
			gch = GCHandle.Alloc (this);
		}

		internal ObjListEqualFuncNative NativeDelegate;
		PackageKit.ObjListEqualFunc managed;

		public ObjListEqualFuncWrapper (PackageKit.ObjListEqualFunc managed)
		{
			this.managed = managed;
			if (managed != null)
				NativeDelegate = new ObjListEqualFuncNative (NativeCallback);
		}

		public static PackageKit.ObjListEqualFunc GetManagedDelegate (ObjListEqualFuncNative native)
		{
			if (native == null)
				return null;
			ObjListEqualFuncWrapper wrapper = (ObjListEqualFuncWrapper) native.Target;
			if (wrapper == null)
				return null;
			return wrapper.managed;
		}
	}
#endregion
}