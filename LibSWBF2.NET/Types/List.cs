using System;
using System.Runtime.InteropServices;
using LibSWBF2.Logging;

namespace LibSWBF2.Types
{
    namespace List
    {
        // For some reason, the native struct needs to be defined outside the generic C# class...
        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        internal struct NativeStruct
        {
            // For more details see BacseChunk.NativeStruct.__vfptr
            //private IntPtr __vfptr;   // NOT ADDED HERE BY C++ ?

            public IntPtr ArrayPtr;
            public ulong NumElements;
            public ulong MaxElements;
        }
    }

    public unsafe class List<T>
    {
        private List.NativeStruct* Ptr = null;

        internal List(List.NativeStruct* ptr)
        {
            Ptr = ptr;
        }

        public T this[int key]
        {
            get
            {
                if (key < 0 || (ulong)key >= Ptr->NumElements)
                {
                    Logger.Log("Index " + key + " is out of bounds! (0-" + Ptr->NumElements + ")", ELogType.Error);
                    return default(T);
                }
                return default(T);
            }
        }
    }
}
