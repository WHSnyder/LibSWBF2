﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using LibSWBF2.Logging;

namespace LibSWBF2.Wrappers
{
    public class Terrain : NativeWrapper
    {
        internal Terrain(IntPtr terrainPtr) : base(terrainPtr)
        {

        }

        public int width;
        public int height;

        public List<string> Names
        {
            get 
            {
                if (!IsValid()) throw new Exception("Underlying native class is destroyed!");
                APIWrapper.Terrain_GetTexNames(NativeInstance, out uint numTextures, out IntPtr strings);

                List<string> textureNames = new List<string>();

                if (numTextures > 0)
                {
                    IntPtr[] stringPointers = new IntPtr[numTextures];
                    Marshal.Copy(strings, stringPointers, 0, (int) numTextures);

                    for (uint i = 0; i < numTextures; i++)
                    {
                        string texName = Marshal.PtrToStringAnsi(stringPointers[i]);
                        textureNames.Add(texName);
                    }

                }  

                return textureNames;
            }
        }

        public float[] Vertices
        {
            get
            {
                if (!IsValid()) throw new Exception("Underlying native class is destroyed!");
                APIWrapper.Terrain_GetVerts(NativeInstance, out uint numVerts, out IntPtr vertsNative);

                float[] rawVerts = new float[((int)numVerts) * 3];
                Marshal.Copy(vertsNative, rawVerts, 0, (int) numVerts * 3);
                return rawVerts;
            }
        }

        public float[] Heights
        {
            get
            {
                if (!IsValid()) throw new Exception("Underlying native class is destroyed!");
                APIWrapper.Terrain_GetHeights(NativeInstance, out uint _width, out uint _height, out IntPtr heightsNative);
                width = (int) _width;
                height = (int) _width;

                Console.WriteLine("Heights width = " + _width);
                Console.WriteLine("Heights height = " + _height);

                float[] heights = new float[(int) width * width];
                Marshal.Copy(heightsNative, heights, 0, (int) width * width);
                return heights;
            }
        }
        
        /*
        public float[] GetHeights(out uint width, out uint height, out IntPtr heightsNative)
        {
            if (!IsValid()) throw new Exception("Underlying native class is destroyed!");
            APIWrapper.Terrain_GetHeights(NativeInstance, out uint width, out uint height, out IntPtr heightsNative);

            float[] heights = new float[(int) 256 * 81];
            Marshal.Copy(heightsNative, heights, 0, (int) heights.Length);
            return heights;
        }
        */

        public int[] Indicies
        {
            get
            {
                if (!IsValid()) throw new Exception("Underlying native class is destroyed!");
                APIWrapper.Terrain_GetIndicies(NativeInstance, out uint numInds, out IntPtr indiciesNative);

                int[] rawInds = new int[(int) numInds];
                Marshal.Copy(indiciesNative, rawInds, 0, (int) numInds);
                return rawInds;
            }
        }
    }
}