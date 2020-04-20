using System;
using System.Runtime.InteropServices;

namespace LibSWBF2.Types
{
    public unsafe class Vector3
    {
        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        internal struct NativeStruct
        {
            // For more details see BacseChunk.NativeStruct.__vfptr
            private IntPtr __vfptr;

            public float X;
            public float Y;
            public float Z;
        }

        private NativeStruct* Ptr = null;

        public float X
        {
            get { return Ptr->X; }
            set { Ptr->X = value; }
        }
        public float Y
        {
            get { return Ptr->Y; }
            set { Ptr->Y = value; }
        }
        public float Z
        {
            get { return Ptr->Z; }
            set { Ptr->Z = value; }
        }

        internal Vector3(NativeStruct* ptr)
        {
            Ptr = ptr;
        }
    }
}
